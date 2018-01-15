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

#ifndef __ccxx_SerialPort_hxx
#define __ccxx_SerialPort_hxx

#include <commonc++/Common.h++>
#include <commonc++/Stream.h++>
#include <commonc++/String.h++>

namespace ccxx {

/**
 * A class representing a serial communications port.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API SerialPort : public Stream
{
 public:

  /** Baud rates. */
  enum BaudRate { BaudOther = -1, Baud300 = 300, Baud600 = 600,
                  Baud1200 = 1200, Baud1800 = 1800, Baud2400 = 2400,
                  Baud4800 = 4800, Baud9600 = 9600, Baud19200 = 19200,
                  Baud38400 = 38400, Baud57600 = 57600,
                  Baud115200 = 115200, Baud230400 = 230400 };

  /** Parity types. */
  enum Parity { ParityOther = -1, ParityNone, ParityOdd, ParityEven,
                ParityMark, ParitySpace };

  /** Data bits per byte. */
  enum DataBits { DataBitsOther = -1, DataBits5 = 5, DataBits6 = 6,
                  DataBits7 = 7, DataBits8 = 8 };

  /** Stop bits. */
  enum StopBits { StopBitsOther = -1, StopBits1 = 1, StopBits2 = 2,
                  StopBits1_5 = 3 };

  /** Flow control modes. */
  enum FlowControl { FlowOther = -1, FlowOff, FlowHardware, FlowXOnXOff };

  /**
   * Construct a new SerialPort for the given device.
   *
   * @param device The device: a serial device file like
   * "/dev/ttyS0" on Linux or "/dev/cu.serial0" on Mac OS X, or a
   * label like "\\.\COM1" on Windows.
   */
  SerialPort(const String& device);

  /** Destructor. Closes the serial port connection. */
  virtual ~SerialPort();

  /**
   * Open the serial port connection. This method must be called before any
   * of the get*() or set*() methods can be used.
   *
   * @throw IOException If an I/O error occurs.
   */
  virtual void open();

  /**
   * Flush any pending I/O on the connection.
   *
   * @throw IOException If an I/O error occurs, or if the conneciton is not
   * open.
   */
  void flush();

  /**
   * Set the stream I/O timeout, in milliseconds. A timeout of 0
   * indicates no timeout, and a timeout of -1 indicates infinite
   * timeout (blocking I/O).
   */
  void setTimeout(timespan_ms_t timeout);

  /**
   * Set the baud rate for the connection.
   *
   * @param baud The new baud rate.
   * @throw IOException If an I/O error occurs, or if the supplied baud rate
   * is <code>BaudOther</code>, or if the connection is not open.
   */
  void setBaudRate(BaudRate baud);

  /**
   * Get the baud rate for the connection.
   *
   * @return The current baud rate; <code>BaudOther</code> if the baud rate
   * is not one of the predefined enum values.
   * @throw IOException If an I/O error occurs, or if the connection is not
   * open.
   */
  BaudRate getBaudRate();

  /**
   * Set the parity type for the connection.
   *
   * @param parity The new parity type.
   * @throw IOException If an I/O error occurs, or if the supplied parity
   * type is <code>ParityOther</code>, or if the connection is not open.
   */
  void setParity(Parity parity);

  /**
   * Get the parity type for the connection.
   *
   * @return The current parity type; <code>ParityOther</code> if the parity
   * type is not one of the predefined enum values.
   * @throw IOException If an I/O error occurs, or if the connection is not
   * open.
   */
  Parity getParity();

  /**
   * Set the number of data bits for the connection.
   *
   * @param bits The new data bits value.
   * @throw IOException If an I/O error occurs, or if the supplied data bits
   * value is <code>DataBitsOther</code>, or if the connection is not open.
   */
  void setDataBits(DataBits bits);

  /**
   * Get the data bits value for the connection.
   *
   * @return The current data bits value; <code>DataBitsOther</code> if the
   * value is not one of the predefined enum values.
   * @throw IOException If an I/O error occurs, or if the connection is not
   * open.
   */
  DataBits getDataBits();

  /**
   * Set the number of stop bits for the connection.
   *
   * @param bits The new stop bits value.
   * @throw IOException If an I/O error occurs, or if the supplied stop bits
   * value is <code>StopBitsOther</code>, or if the connection is not open.
   */
  void setStopBits(StopBits bits);

  /**
   * Get the stop bits value for the connection.
   *
   * @return The current stop bits value; <code>StopBitsOther</code> if the
   * value is not one of the predefined enum values.
   * @throw IOException If an I/O error occurs, or if the connection is not
   * open.
   */
  StopBits getStopBits();

  /**
   * Set the flow control mode for the connection.
   *
   * @param flow The new flow control mode.
   * @throw IOException If an I/O error occurs, or if the supplied flow
   * control mode is <code>FlowControlOther</code>, or if the connection is
   * not open.
   */
  void setFlowControl(FlowControl flow);

  /**
   * Get the flow control mode for the connection.
   *
   * @return The current flow control mode; <code>FlowControlOther</code> if
   * the flow control mode is not one of the predefined enum values.
   * @throw IOException If an I/O error occurs, or if the connection is not
   * open.
   */
  FlowControl getFlowControl();

  /**
   * Send a break on the serial port for the specified amount of time.
   *
   * @param duration The duration. On Windows, the duration is
   * specified in milliseconds. On other platforms, the meaning of
   * the value is implementation-defined.
   */
  void sendBreak(uint_t duration);

  /** Get the device for this serial port. */
  inline String getDevice() const
  { return(_device); }

  /** Get the BaudRate enumeration value for the given baud rate value.
   *
   * @param baudRate A baud rate.
   * @return The BaudRate value that is closest to, but not greater than,
   * the given baud rate.
   */
  static BaudRate lookupBaudRate(uint_t baudRate);

 private:

  String _device;

  CCXX_COPY_DECLS(SerialPort);
};

} // namespace ccxx

#endif // __ccxx_SerialPort_hxx
