import socket
import threading
import select
from random import randint
import struct

PORT = 4567
IP = "127.0.0.1"
BUFFER_SIZE = 65535
running = True

class Msg:
    def __init__(self, type:str):
        self.type = type
        self.cont = ""

class UDPMsgFactory:
    def confirm(self, ref_id: int) -> bytes:
        return struct.pack("!BH", 0x00, ref_id)

    def reply(self, msg_id: int, result: int, ref_id: int, content: str) -> bytes:
        return struct.pack(f"!BHBH{len(content)}sB", 0x01, msg_id & 0xFFFF, result, ref_id, content.encode(), 0)

    def msg(self, msg_id: int, display_name: str, content: str) -> bytes:
        return struct.pack(
            f"!BH{len(display_name)}sB{len(content)}sB", 
            0x04, msg_id, display_name.encode(), 0, 
            content.encode(), 0
        )

    def err(self, msg_id: int, display_name: str, content: str) -> bytes:
        return struct.pack(
            f"!BH{len(display_name)}sB{len(content)}sB", 
            0xFE, msg_id, display_name.encode(), 0, 
            content.encode(), 0
        )

    def bye(self, msg_id: int, display_name: str) -> bytes:
        return struct.pack(
            f"!BH{len(display_name)}sB", 
            0xFF, msg_id, display_name.encode(), 0
        )

class ClientBase:
    def __init__(self, addr):
        self.addr = addr
        self.state = "START"
        self.display_name = "unknown"
        self.TCP = False

    def parse_msg(self, data):
        raise NotImplementedError

    def send(self, message):
        raise NotImplementedError
    
    def create_msg(self, type: str, content=""):
        raise NotImplementedError

    def handle(self, data):
        msg = self.parse_msg(data)
        self.transition(msg)

    def transition(self, msg: Msg):
        if msg.type == "INVALID":
            self.send(self.create_msg("ERR", "Invalid mmessage received"))
            raise RuntimeError
        if msg.type == "CONFIRM":
            return
        match self.state:
            case "START":
                if msg.type == "AUTH":
                    if msg.cont == "nreply":
                        self.send(self.create_msg("NREPLY", "auth error"))
                    elif msg.cont == "err":
                        self.send(self.create_msg("ERR", "error"))
                        raise RuntimeError
                    elif msg.cont == "bye":
                        self.send(self.create_msg("BYE"))
                        raise RuntimeError
                    elif msg.cont == "msg":
                        self.send(self.create_msg("MSG", "msg"))
                    elif msg.cont == "timeout":
                        pass
                    else:
                        self.send(self.create_msg("REPLY", "auth OK"))
                        self.state = "OPEN"
                elif msg.type == "ERR":
                    raise RuntimeError
                elif msg.type == "BYE":
                    raise RuntimeError
                else:
                    self.send(self.create_msg("ERR", "unknown msg"))
                    raise RuntimeError
            case "OPEN":
                if msg.type == "MSG":
                    if msg.cont == "reply":
                        self.send(self.create_msg("REPLY", "reply"))
                    elif msg.cont == "msg":
                        self.send(self.create_msg("MSG", "msg"))
                    elif msg.cont == "err":
                        self.send(self.create_msg("ERR", "error"))
                        raise RuntimeError
                    elif msg.cont == "bye":
                        self.send(self.create_msg("BYE"))
                        raise RuntimeError
                    elif msg.cont in ["spec1", "spec2", "spec3", "mal1", "mal2", "mal3", "mal4", "mal5", "mal6"]:
                        self.send(self.create_msg("MSG", msg.cont))
                elif msg.type == "JOIN":
                    if msg.cont == "err":
                        self.send(self.create_msg("ERR", "error"))
                        raise RuntimeError
                    elif msg.cont == "bye":
                        self.send(self.create_msg("BYE"))
                        raise RuntimeError
                    elif msg.cont == "timeout":
                        pass
                    else:
                        self.send(self.create_msg("MSG", "welcome"))
                        self.send(self.create_msg("REPLY", "join OK"))
                elif msg.type == "ERR":
                    raise RuntimeError
                elif msg.type == "BYE":
                    raise RuntimeError
                else:
                    self.send(self.create_msg("ERR", "unknown msg"))
                    raise RuntimeError
            case _:
                raise RuntimeError 

class TCPClient(ClientBase):
    def __init__(self, sock: socket, addr):
        super().__init__(addr)
        self.sock = sock
        self.TCP = True

    def parse_msg(self, data: str) -> Msg:
        data = data.strip().split()
        msg = Msg(data[0])
        match data[0]:
            case "AUTH":
                self.display_name = data[3]
                msg.cont = data[5]
            case "JOIN":
                msg.cont = data[1]
                self.display_name = data[3]
            case "MSG":
                msg.cont = data[4]
                self.display_name = data[2]
            case "ERR":
                msg.cont = data[4]
                self.display_name = data[2]
            case "BYE":
                self.display_name = data[2]
            case _:
                msg.type = "UNKNOWN"
        return msg

    def create_msg(self, type: str, content=""):
        if type == "REPLY":
            return f"REPlY OK Is {content}\r\n"
        elif type == "NREPLY":
            return f"rEPLY NOK IS {content}\r\n"
        elif type == "ERR":
            return f"ERR FROM Server IS {content}\r\n"
        elif type == "BYE":
            return f"ByE FROM Server\r\n"
        else:
            if content == "spec1":          #1 v 2
                self.send(f"MSG FROM Ser")
                return f"ver Is spec1\r\n"
            elif content == "spec2":          #2 v 1
                return f"MSG FROM Server IS spec21\r\nMSG FRom Server is spec2\r\n"
            elif content == "spec3":            # 2 v 3
                self.send(f"MSG FROM Server ")
                self.send(f"IS spec31\r\nMSG from Server is spec3\r")
                return f"\n"
            elif content == "mal1":
                return f"MSG From Server i s mal1\r\n"
            elif content == "mal2":
                return f"MSG FROM IS mal2\r\n"
            elif content == "mal3":
                return f"MSG FROM ServerServerServerSer IS mal3\r\n"
            elif content == "mal4":
                return f"MSG FROM Ser\nver IS mal3\r\n"
            elif content == "mal5":
                return f"\r\n"
            elif content == "mal6":
                return f"Bye from server \r\n"
            else:
                return f"MSG FROM Server IS {content}\r\n"

    def send(self, message: str):
        self.sock.sendall(message.encode())

class UDPClient(ClientBase):
    def __init__(self, addr):
        super().__init__(addr)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((IP, 0))
        self.msg_id = randint(0, 65535)
        self.msg_fact = UDPMsgFactory()
        self.last_id = -1

    def handle(self, data):
        msg = self.parse_msg(data)
        if msg.type != "CONFIRM" and msg.cont != "mal1":
            self.last_id = msg.id
            self.sock.sendto(self.msg_fact.confirm(msg.id), self.addr)
        if msg.type == "MSG" and msg.cont == "spec2":
            self.send(self.msg_fact.msg(self.msg_id, "Server", "spec21"))
        self.transition(msg)

    def parse_msg(self, data: bytes) -> Msg:
        if len(data) < 3:
            return Msg("INVALID")

        msg_type = data[0]
        msg_id = int.from_bytes(data[1:3], "big")
        msg = Msg("UNKNOWN")
        msg.id = msg_id
        if msg_type == 0x00:
            msg.type = "CONFIRM"
        elif msg_type == 0x02:
            if len(data) < 4:
                msg.type = "INVALID"
                return msg
            parts = data[3:].split(b'\x00', maxsplit=2)
            if len(parts) != 3:
                msg.type = "INVALID"
                return msg
            self.display_name = parts[1][:-1].decode("ascii", errors="ignore")
            msg.cont = parts[2][:-1].decode("ascii", errors="ignore")
            msg.type = "AUTH"
        elif msg_type == 0x03:
            if len(data) < 4:
                msg.type = "INVALID"
                return msg
            parts = data[3:].split(b'\x00', maxsplit=1)
            if len(parts) != 2:
                msg.type = "INVALID"
                return msg
            msg.type = "JOIN"
            msg.cont = parts[0].decode("ascii", errors="ignore")
            self.display_name =  parts[1][:-1].decode("ascii", errors="ignore")

        elif msg_type == 0x04:
            parts = data[3:].split(b'\x00', maxsplit=1)
            if len(parts) != 2:
                msg.type = "INVALID"
                return msg
            self.display_name = parts[0][:-1].decode("ascii", errors="ignore")
            msg.cont = parts[1][:-1].decode("ascii", errors="ignore")
            msg.type = "MSG"
        elif msg_type == 0xFE:
            msg.type = "ERR"
        elif msg_type == 0xFF:
            msg.type = "BYE"
        return msg

    def create_msg(self, type: str, content=""):
        if type == "REPLY":
            return self.msg_fact.reply(self.msg_id, 1, self.last_id, content)
        elif type == "NREPLY":
            return self.msg_fact.reply(self.msg_id, 0, self.last_id, content)
        elif type == "ERR":
            return self.msg_fact.err(self.msg_id, "Server", content)
        elif type == "BYE":
            return self.msg_fact.bye(self.msg_id, "Server")
        elif type == "MSG":
            server = "Server"
            if content == "spec1":
                self.send(self.msg_fact.msg(self.msg_id, "Server", "spec1"))
                self.msg_id -= 1
            elif content == "spec3":
                self.send(self.msg_fact.msg(self.msg_id, "Server", "spec31"))
            elif content == "mal2":
                return struct.pack(f"!BH{len(server)}sB{len(content)}sBB", 0x04, self.msg_id, server.encode(), 0, content.encode(), 0, 0)
            elif content == "mal3":
                return struct.pack(f"!BHB{len(content)}sB", 0x04, self.msg_id, 0, content.encode(), 0)
            elif content == "mal4":
                return self.msg_fact.msg(self.msg_id, "ServerServerServerSer", content)
            elif content == "mal5":
                return struct.pack(f"!BH{len(server)}sB{len(content)}s", 0x04, self.msg_id, server.encode(), 0, content.encode())
            elif content == "mal6":
                return struct.pack(f"!BH", 0x04, self.msg_id)
            return self.msg_fact.msg(self.msg_id, "Server", content)
        elif type == "CONFIRM":
            return self.msg_fact.confirm(content)

    def send(self, message, retries=3, timeout=0.25):
        for _ in range(retries):
            self.sock.sendto(message, self.addr)
            self.sock.settimeout(timeout)
            try:
                response, _ = self.sock.recvfrom(1024)
                msg = self.parse_msg(response)
                if msg.type == "CONFIRM" and msg.id == self.msg_id:
                    self.msg_id += 1
                    return
            except socket.timeout:
                pass
        print(f"[UDP] Client {self.addr} is not responding")
        raise RuntimeError

def handle_tcp_client(client: TCPClient):
    global running
    print(f"[TCP] New connection from {client.addr}")
    with client.sock:
        try:
            while running:
                data = client.sock.recv(BUFFER_SIZE).decode('ascii')
                if not data:
                    break
                client.handle(data)
        except RuntimeError:
            pass
    print(f"[TCP] Client {client.addr} disconnected")

def handle_udp_client(client: UDPClient, data):
    global running
    print(f"[UDP] New connection from {client.addr}")
    with client.sock:
        try:
            client.handle(data)
            while running:
                try:
                    data, _ = client.sock.recvfrom(1024)
                    client.handle(data)
                except socket.timeout:
                    continue
        except RuntimeError:
            pass
    print(f"[UDP] Client {client.addr} disconnected")

def main():
    global running 
    tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    tcp_sock.bind((IP, PORT))
    tcp_sock.listen()

    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind((IP, PORT))

    print(f"[INFO] Server listening on TCP {PORT} and UDP {PORT}")
    try:
        while True:
            read_list = [tcp_sock, udp_sock]
            readable, _, _ = select.select(read_list, [], [])

            for s in readable:
                if s is tcp_sock:
                    sock, addr = tcp_sock.accept()
                    client = TCPClient(sock, addr)
                    threading.Thread(target=handle_tcp_client, args=(client, )).start()
                elif s is udp_sock:
                    data, addr = udp_sock.recvfrom(BUFFER_SIZE)
                    client = UDPClient(addr)
                    threading.Thread(target=handle_udp_client, args=(client, data)).start()
    except KeyboardInterrupt:
        running = False

if __name__ == "__main__":
    main()
