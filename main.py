from socket import *
import base64


def sendMess(message):
    clientSocket.send(message.encode('utf-8'))
    rep = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not rep[:3] == '250':
        print('250 reply not received from server.')



if __name__ == '__main__':
    # Choose a mail server (e.g. Google mail server) and call it mail server
    # msg = input('Please input message: ')
    # img = input('Please input image name: ')
    # receiver = input('Please input receiver mail: ')
    msg = 'test for sending emails'
    img = 'shoot.png'
    receiver = '13750948165@163.com'
    mailServer = 'smtp.qq.com'
    port = 25

    # TODO: Sender and receiver
    sender = '814750204@qq.com'
    senderAuth = 'okpqtyystdrgbeeb'

    # TODO: Auth information (Encode with base64)
    senderBase64 = base64.b64encode(sender.encode('utf-8'))
    senderAuthBase = base64.b64encode(senderAuth.encode('utf-8'))

    # TODO: Create socket called clientSocket and establish a TCP connection with mail server
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((mailServer, port))

    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '220':
        print('220 reply not received from server.')

    # TODO: Send HELO command and print server response.
    helloCommand = 'HELO Lincyaw\r\n'
    print('client: ' + helloCommand)
    clientSocket.send(helloCommand.encode('utf-8'))
    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '250':
        print('250 reply not received form server.')

    # TODO: Send MAIL FROM command and print server response.
    command = 'auth login\r\n'
    print('client: ' + command)
    sendMess(command)

    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '334':
        print('334 reply not received from server.')

    clientSocket.send(senderBase64)
    clientSocket.send('\r\n'.encode('utf-8'))

    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '334':
        print('when input username, 334 reply not received from server.')

    clientSocket.send(senderAuthBase)
    clientSocket.send('\r\n'.encode('utf-8'))

    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '235':
        print('when input password, 235 reply not received from server.')

    # TODO: Send RCPT TO command and print server response.
    mailFrom = 'mail from: <' + sender + '> \r\n'
    sendMess(mailFrom)

    rcpt = 'rcpt to: <' + receiver + '> \r\n'
    sendMess(rcpt)

    # TODO: Send DATA command and print server response.
    dataCmd = 'data\r\n'
    print('client: ' + dataCmd)
    clientSocket.send(dataCmd.encode('utf-8'))
    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '354':
        print('354 reply not received from server.')

    # TODO: Send message data.
    fromWhere = 'from: <' + sender + '> \r\n'
    toWhere = 'to: <' + receiver + '> \r\n'
    subject = "subject:I love computer networks!" + '\r\n'
    mimeVersion = 'MIME-Version:1.0' + '\r\n'
    lines = 'lines'
    contentType = 'Content-Type:multipart/mixed;boundary=' + lines + '\r\n' + '\r\n'
    head = fromWhere + toWhere + subject + mimeVersion + contentType

    # TODO: Message ends with a single period.
    msgHead = '--' + lines + '\r\n' + 'Content-Type:text/plain' + '\r\n' + '\r\n'
    msg = msg + '\r\n'
    clientSocket.send((head + msgHead + msg).encode('utf-8'))

    if not img == '':
        header = '--' + lines + '\r\n' + 'Content-Type:image/jpeg; name=img.jpg' + '\r\n' +\
                 'Content-Transfer-Encoding: ' + 'base64' + '\r\n' + '\r\n '
        with open(img, 'rb') as f:
            imgBase = base64.b64encode(f.read())
        end = '\r\n' + '--' + lines + '--' + '\r\n'
        clientSocket.send(header.encode('utf-8'))
        clientSocket.send(imgBase)
        clientSocket.send(end.encode('utf-8'))

    endMsg = "\r\n.\r\n"
    clientSocket.send(endMsg.encode('utf-8'))

    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '250':
        print('250 reply not received from server.')

    # TODO: Send QUIT command and get server response.
    quitCmd = 'quit\r\n'
    clientSocket.send(quitCmd.encode('utf-8'))
    recv = clientSocket.recv(1024).decode('utf-8')
    print('server: ' + recv)
    if not recv[:3] == '221':
        print('221 reply not received from server.')
