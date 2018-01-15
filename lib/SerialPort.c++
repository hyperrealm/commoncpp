/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
*/

#include "commonc++/SerialPort.h++"
#include "commonc++/System.h++"
#include "commonc++/Permissions.h++"

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#ifdef CCXX_OS_ANDROID
#define _POSIX_VDISABLE _PC_VDISABLE
#endif

#endif

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"
#endif

namespace ccxx {

/*
 */

SerialPort::SerialPort(const String& device)
  : _device(device)
{
}

/*
 */

SerialPort::~SerialPort()
{
}

/*
 */

void SerialPort::open()
{
  const Permissions perm = Permissions::USER_READ_WRITE;

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Windows::encodePermissions(perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

  FileHandle r = ::CreateFileW(_device.data(),
                               (GENERIC_READ | GENERIC_WRITE),
                               (FILE_SHARE_READ | FILE_SHARE_WRITE), &sa,
                               OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

  if(r == INVALID_HANDLE_VALUE)
  {
    int err = ::GetLastError();
    if((err == ERROR_FILE_NOT_FOUND) || (err == ERROR_PATH_NOT_FOUND))
      throw PathNotFoundException(_device);
    else
      throw IOException(System::getErrorString("CreateFile"));
  }

  if(! ::FlushFileBuffers(r))
  {
    String s = System::getErrorString("FlushFileBuffers");
    ::CloseHandle(r);
    throw IOException(s);
  }

  if(! ::PurgeComm(r, (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
                       | PURGE_RXCLEAR)))
  {
    String s = System::getErrorString("PurgeComm");
    ::CloseHandle(r);
    throw IOException(s);
  }

#else

  mode_t perm_;
  POSIX::encodePermissions(perm, perm_);

  CString cstr_device = _device.toUTF8();
  FileHandle r = ::open(cstr_device.data(), (O_RDWR | O_NOCTTY | O_NONBLOCK),
                        perm_);
  if(r < 0)
  {
    if(errno == ENOENT)
      throw PathNotFoundException(_device);
    else
      throw IOException(System::getErrorString("open"));
  }

  if(::tcflush(r, TCIOFLUSH) != 0)
  {
    String s = System::getErrorString("tcflush");
    ::close(r);
    throw IOException(s);
  }

  struct termios ts;

  if(::tcgetattr(r, &ts) != 0)
  {
    String s = System::getErrorString("tcgetattr");
    ::close(r);
    throw IOException(s);
  }

  ts.c_cflag |= (CREAD | CLOCAL);
  ts.c_iflag = IGNPAR;
  ts.c_oflag = 0;
  ts.c_lflag = 0;
  ts.c_cc[VINTR]  = _POSIX_VDISABLE;
  ts.c_cc[VQUIT]  = _POSIX_VDISABLE;
  ts.c_cc[VSTART] = _POSIX_VDISABLE;
  ts.c_cc[VSTOP]  = _POSIX_VDISABLE;
  ts.c_cc[VSUSP]  = _POSIX_VDISABLE;
  ts.c_cc[VEOF]   = _POSIX_VDISABLE;
  ts.c_cc[VEOL]   = _POSIX_VDISABLE;
  ts.c_cc[VERASE] = _POSIX_VDISABLE;
  ts.c_cc[VKILL]  = _POSIX_VDISABLE;

  if(::tcsetattr(r, TCSANOW, &ts) != 0)
  {
    ::close(r);
    throw IOException(System::getErrorString("tcsetattr"));
  }

#endif

  Stream::_init(r, false, true, true);

  flush();
}

/*
 */

void SerialPort::flush()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  if(! ::FlushFileBuffers(_handle))
    throw IOException(System::getErrorString("FlushFileBuffers"));

  if(! ::PurgeComm(_handle, (PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR
                             | PURGE_RXCLEAR)))
    throw IOException(System::getErrorString("PurgeComm"));

#else

  if(::tcflush(_handle, TCIOFLUSH) != 0)
    throw IOException(System::getErrorString("tcflush"));

#endif
}

/*
 */

void SerialPort::setTimeout(timespan_ms_t timeout)
{
#ifdef CCXX_OS_WINDOWS

  COMMTIMEOUTS timeouts = { 0, 0, 0, 0, 0 };

  if(timeout >= 0)
  {
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = (timeout > 0 ? MAXDWORD : 0);
    timeouts.ReadTotalTimeoutConstant = static_cast<DWORD>(timeout);
  }

  if(! ::SetCommTimeouts(_handle, &timeouts))
    throw IOException(System::getErrorString("SetCommTimeouts"));

#endif

  Stream::setTimeout(timeout);
}

/*
 */

void SerialPort::setBaudRate(SerialPort::BaudRate baud)
{
  if(! isOpen())
    throw IOException("stream not open");

  if(baud == BaudOther)
    throw IOException("unsupported value for parameter");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  switch(baud)
  {
    case Baud300:
      dcb.BaudRate = CBR_300;
      break;

    case Baud600:
      dcb.BaudRate = CBR_600;
      break;

    case Baud1200:
      dcb.BaudRate = CBR_1200;
      break;

    case Baud1800:
      dcb.BaudRate = 1800;
      break;

    case Baud2400:
      dcb.BaudRate = CBR_2400;
      break;

    case Baud4800:
      dcb.BaudRate = CBR_4800;
      break;

    case Baud9600:
      dcb.BaudRate = CBR_9600;
      break;

    case Baud19200:
      dcb.BaudRate = CBR_19200;
      break;

    case Baud38400:
      dcb.BaudRate = CBR_38400;
      break;

    case Baud57600:
      dcb.BaudRate = CBR_57600;
      break;

    case Baud115200:
      dcb.BaudRate = CBR_115200;
      break;

    case Baud230400:
      dcb.BaudRate = 230400;
      break;
  }

  if(! ::SetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("SetCommState"));

#else

  struct termios ts;
  speed_t speed = B0;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  switch(baud)
  {
    case Baud300:
      speed = B300;
      break;

    case Baud600:
      speed = B600;
      break;

    case Baud1200:
      speed = B1200;
      break;

    case Baud1800:
      speed = B1800;
      break;

    case Baud2400:
      speed = B2400;
      break;

    case Baud4800:
      speed = B4800;
      break;

    case Baud9600:
      speed = B9600;
      break;

    case Baud19200:
      speed = B19200;
      break;

    case Baud38400:
      speed = B38400;
      break;

    case Baud57600:
      speed = B57600;
      break;

    case Baud115200:
      speed = B115200;
      break;

    case Baud230400:
      speed = B230400;
      break;

    default:
      throw IOException("invalid baud rate");
  }

  ::cfsetispeed(&ts, speed);
  ::cfsetospeed(&ts, speed);

  if(::tcsetattr(_handle, TCSANOW, &ts) != 0)
    throw IOException(System::getErrorString("tcsetattr"));

#endif
}

/*
 */

SerialPort::BaudRate SerialPort::getBaudRate()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  BaudRate baud;

  switch(dcb.BaudRate)
  {
    case CBR_300:
      baud = Baud300;
      break;

    case CBR_600:
      baud = Baud600;
      break;

    case CBR_1200:
      baud = Baud1200;
      break;

    case CBR_2400:
      baud = Baud2400;
      break;

    case CBR_4800:
      baud = Baud4800;
      break;

    case CBR_9600:
      baud = Baud9600;
      break;

    case CBR_19200:
      baud = Baud19200;
      break;

    case CBR_38400:
      baud = Baud38400;
      break;

    case CBR_57600:
      baud = Baud57600;
      break;

    case CBR_115200:
      baud = Baud115200;
      break;

    case 230400:
      baud = Baud230400;
      break;

    default:
      baud = BaudOther;
      break;
  }

  return(baud);

#else

  struct termios ts;
  BaudRate baud;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  speed_t speed = ::cfgetispeed(&ts);
  speed_t ospeed = ::cfgetospeed(&ts);

  if(speed != ospeed)
    return(BaudOther);

  switch(speed)
  {
    case B300:
      baud = Baud300;
      break;

    case B600:
      baud = Baud600;
      break;

    case B1200:
      baud = Baud1200;
      break;

    case B2400:
      baud = Baud2400;
      break;

    case B4800:
      baud = Baud4800;
      break;

    case B9600:
      baud = Baud9600;
      break;

    case B19200:
      baud = Baud19200;
      break;

    case B38400:
      baud = Baud38400;
      break;

    case B57600:
      baud = Baud57600;
      break;

    case B115200:
      baud = Baud115200;
      break;

    case B230400:
      baud = Baud230400;
      break;

    default:
      baud = BaudOther;
      break;
  }

  return(baud);

#endif
}

/*
 */

void SerialPort::setParity(SerialPort::Parity parity)
{
  if(! isOpen())
    throw IOException("stream not open");

  if(parity == ParityOther)
    throw IOException("unsupported value for parameter");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  switch(parity)
  {
    case ParityNone:
      dcb.Parity = NOPARITY;
      break;

    case ParityOdd:
      dcb.Parity = ODDPARITY;
      break;

    case ParityEven:
      dcb.Parity = EVENPARITY;
      break;

    case ParityMark:
      dcb.Parity = MARKPARITY;
      break;

    case ParitySpace:
      dcb.Parity = SPACEPARITY;
      break;

    default:
      break;
  }

  if(! ::SetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("SetCommState"));

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  switch(parity)
  {
    case ParityNone:
    case ParitySpace:
      ts.c_cflag &= ~PARENB;
      break;

    case ParityOdd:
      ts.c_cflag |= (PARENB | PARODD);
      break;

    case ParityEven:
      ts.c_cflag |= PARENB;
      ts.c_cflag &= ~PARODD;
      break;

    case ParityMark:
      ts.c_cflag &= ~PARENB;
      ts.c_cflag |= CSTOPB;
      ts.c_cflag &= ~CSIZE;
      ts.c_cflag |= CS7;
      break;

    default:
      break;
  }

  if(::tcsetattr(_handle, TCSANOW, &ts) != 0)
    throw IOException(System::getErrorString("tcsetattr"));

#endif
}

/*
 */

SerialPort::Parity SerialPort::getParity()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  Parity parity;

  switch(dcb.Parity)
  {
    case NOPARITY:
      parity = ParityNone;
      break;

    case ODDPARITY:
      parity = ParityOdd;
      break;

    case EVENPARITY:
      parity = ParityEven;
      break;

    case MARKPARITY:
      parity = ParityMark;
      break;

    case SPACEPARITY:
      parity = ParitySpace;
      break;

    default:
      parity = ParityOther;
  }

  return(parity);

#else

  struct termios ts;
  Parity parity = ParityNone;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  if((ts.c_cflag & PARENB) != 0)
    parity = ((ts.c_cflag & PARODD) != 0) ? ParityOdd : ParityEven;

  return(parity);

#endif
}

/*
 */

void SerialPort::setDataBits(SerialPort::DataBits bits)
{
  if(! isOpen())
    throw IOException("stream not open");

  if(bits == DataBitsOther)
    throw IOException("unsupported value for parameter");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  switch(bits)
  {
    case DataBits5:
      dcb.ByteSize = 5;
      break;

    case DataBits6:
      dcb.ByteSize = 6;
      break;

    case DataBits7:
      dcb.ByteSize = 7;
      break;

    case DataBits8:
      dcb.ByteSize = 8;
      break;
  }

  if(! ::SetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("SetCommState"));

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  ts.c_cflag &= ~CSIZE;

  switch(bits)
  {
    case DataBits5:
      ts.c_cflag |= CS5;
      break;

    case DataBits6:
      ts.c_cflag |= CS6;
      break;

    case DataBits7:
      ts.c_cflag |= CS7;
      break;

    case DataBits8:
      ts.c_cflag |= CS8;
      break;

    default:
      break;
  }

  if(::tcsetattr(_handle, TCSANOW, &ts) != 0)
    throw IOException(System::getErrorString("tcsetattr"));

#endif
}

/*
 */

SerialPort::DataBits SerialPort::getDataBits()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  DataBits bits;

  switch(dcb.ByteSize)
  {
    case 5:
      bits = DataBits5;
      break;

    case 6:
      bits = DataBits6;
      break;

    case 7:
      bits = DataBits7;
      break;

    case 8:
      bits = DataBits8;
      break;

    default:
      bits = DataBitsOther;
      break;
  }

  return(bits);

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  DataBits bits;

  if((ts.c_cflag & CS5) != 0)
    bits = DataBits5;
  else if((ts.c_cflag & CS6) != 0)
    bits = DataBits6;
  else if((ts.c_cflag & CS7) != 0)
    bits = DataBits7;
  else if((ts.c_cflag & CS8) != 0)
    bits = DataBits8;
  else
    bits = DataBitsOther;

  return(bits);

#endif
}

/*
 */

void SerialPort::setStopBits(SerialPort::StopBits bits)
{
  if(! isOpen())
    throw IOException("stream not open");

  if(bits == StopBitsOther)
    throw IOException("unsupported value for parameter");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  switch(bits)
  {
    case StopBits1:
      dcb.StopBits = ONESTOPBIT;
      break;

    case StopBits1_5:
      dcb.StopBits = ONE5STOPBITS;
      break;

    case StopBits2:
      dcb.StopBits = TWOSTOPBITS;
      break;

    default:
      break;
  }

  if(! ::SetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("SetCommState"));

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  switch(bits)
  {
    case StopBits1:
    case StopBits1_5:
      ts.c_cflag &= ~CSTOPB;
      break;

    case StopBits2:
      ts.c_cflag |= CSTOPB;
      break;

    default:
      break;
  }

  if(::tcsetattr(_handle, TCSANOW, &ts) != 0)
    throw IOException(System::getErrorString("tcsetattr"));

#endif
}

/*
 */

SerialPort::StopBits SerialPort::getStopBits()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  StopBits bits;

  switch(dcb.StopBits)
  {
    case ONESTOPBIT:
      bits = StopBits1;
      break;

    case ONE5STOPBITS:
      bits = StopBits1_5;
      break;

    case TWOSTOPBITS:
      bits = StopBits2;
      break;

    default:
      bits = StopBitsOther;
      break;
  }

  return(bits);

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  StopBits bits = StopBits1;

  if((ts.c_cflag & CSTOPB) != 0)
    bits = StopBits2;

  return(bits);

#endif
}

/*
 */

void SerialPort::setFlowControl(SerialPort::FlowControl flow)
{
  if(! isOpen())
    throw IOException("stream not open");

  if(flow == FlowOther)
    throw IOException("unsupported value for parameter");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  switch(flow)
  {
    case FlowOff:
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      dcb.fInX = FALSE;
      dcb.fOutX = FALSE;
      dcb.fDsrSensitivity = FALSE;
      break;

    case FlowHardware:
      dcb.fOutxCtsFlow = TRUE;
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
      dcb.fInX = FALSE;
      dcb.fOutX = FALSE;
      break;

    case FlowXOnXOff:
      dcb.fOutxCtsFlow = FALSE;
      dcb.fOutxDsrFlow = FALSE;
      dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
      dcb.fRtsControl = RTS_CONTROL_DISABLE;
      dcb.fInX = TRUE;
      dcb.fOutX = TRUE;
      dcb.XonChar = 0x11;
      dcb.XoffChar = 0x13;
      dcb.XoffLim = 100;
      dcb.XonLim = 100;
      break;
  }

  if(! ::SetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("SetCommState"));

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  switch(flow)
  {
    case FlowOff:
      ts.c_cflag &= ~CRTSCTS;
      ts.c_cflag &= ~(IXON | IXOFF | IXANY);
      break;

    case FlowHardware:
      ts.c_cflag |= CRTSCTS;
      ts.c_iflag &= ~(IXON | IXOFF | IXANY);
      break;

    case FlowXOnXOff:
      ts.c_cflag &= ~CRTSCTS;
      ts.c_iflag |= (IXON | IXOFF | IXANY);
      break;

    default:
      break;
  }

  if(::tcsetattr(_handle, TCSANOW, &ts) != 0)
    throw IOException(System::getErrorString("tcsetattr"));

#endif
}

/*
 */

SerialPort::FlowControl SerialPort::getFlowControl()
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  DCB dcb;

  if(! ::GetCommState(_handle, &dcb))
    throw IOException(System::getErrorString("GetCommState"));

  FlowControl flow;

  if((dcb.fOutxCtsFlow == FALSE) && (dcb.fRtsControl == RTS_CONTROL_DISABLE)
     && (dcb.fInX == FALSE) && (dcb.fOutX == FALSE))
    flow = FlowOff;
  else if((dcb.fOutxCtsFlow == TRUE)
          && (dcb.fRtsControl == RTS_CONTROL_HANDSHAKE)
          && (dcb.fInX == FALSE) && (dcb.fOutX == FALSE))
    flow = FlowHardware;
  else if((dcb.fOutxCtsFlow = FALSE)
          && (dcb.fRtsControl == RTS_CONTROL_DISABLE)
          && (dcb.fInX == TRUE) && (dcb.fOutX == TRUE))
    flow = FlowXOnXOff;
  else
    flow = FlowOther;

  return(flow);

#else

  struct termios ts;

  if(::tcgetattr(_handle, &ts) != 0)
    throw IOException(System::getErrorString("tcgetattr"));

  FlowControl flow = FlowOther;

  if((ts.c_cflag & CRTSCTS) != 0)
    flow = FlowHardware;
  else if((ts.c_cflag & (IXON | IXOFF | IXANY)) != 0)
    flow = FlowXOnXOff;
  else if((ts.c_cflag | (IXON | IXOFF | IXANY)) == 0)
    flow = FlowOff;
  else
    flow = FlowOther;

  return(flow);

#endif
}

/*
 */

void SerialPort::sendBreak(uint_t duration)
{
  if(! isOpen())
    throw IOException("stream not open");

#ifdef CCXX_OS_WINDOWS

  ::SetCommBreak(_handle);
  ::Sleep(duration);
  ::ClearCommBreak(_handle);

#else

  ::tcsendbreak(_handle, duration);

#endif
}

/*
 */

SerialPort::BaudRate SerialPort::lookupBaudRate(uint_t baudRate)
{
  if(baudRate <= 300)
    return(Baud300);
  else if(baudRate <= 600)
    return(Baud600);
  else if(baudRate <= 1200)
    return(Baud1200);
  else if(baudRate <= 1800)
    return(Baud1800);
  else if(baudRate <= 2400)
    return(Baud2400);
  else if(baudRate <= 4800)
    return(Baud4800);
  else if(baudRate <= 9600)
    return(Baud9600);
  else if(baudRate <= 19200)
    return(Baud19200);
  else if(baudRate <= 38400)
    return(Baud38400);
  else if(baudRate <= 57600)
    return(Baud57600);
  else if(baudRate <= 115200)
    return(Baud115200);
  else
    return(Baud230400);
}


} // namespace ccxx
