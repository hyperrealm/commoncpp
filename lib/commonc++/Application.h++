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

#ifndef __ccxx_Application_hxx
#define __ccxx_Application_hxx

#include <commonc++/Common.h++>
#include <commonc++/String.h++>

#include <cstddef>

namespace ccxx {

/** Base functionality for an application, including command-line
 * argument parsing and a shutdown handler. This class is implicitly
 * a singleton; there can be only one instantiation of the class per
 * process. The class should be subclassed and instantiated within
 * the program's <b>main()</b> function.
 * <p>
 *
 * @author Mark Lindner
 */
class COMMONCPP_API Application
{
 private:

  class CommandOption
  {
    friend class Application;

   protected:

    char _opt;
    String _longOpt;
    bool _hasArg;
    String _argName;
    String _argDesc;

    CommandOption(char opt, const String& longOpt, const String& argName,
                  const String& argDesc);

    inline bool hasOpt() const
    { return(_opt != NUL); }

    inline bool hasLongOpt() const
    { return(! _longOpt.isNull()); }

    inline char getOpt() const
    { return (_opt); }

    inline String getLongOpt() const
    { return(_longOpt); }

    inline bool hasArg() const { return(_hasArg); }

    inline const String& getArgDesc() const { return(_argDesc); }
    inline const String& getArgName() const { return(_argName); }
  };

  class CommandOptionList; // fwd decl

 public:

  /**
   * Construct a new Application.
   *
   * @param argc The argument count (from <b>main()</b>).
   * @param argv The argument list (from <b>main()</b>).
   * @param version The optional version of the program; if not supplied,
   * it defaults to "0.0".
   * @param compileDate The optional compile date of the program;
   * typically provided by the compiler-defined macro <b>__DATE__</b>.
   * @param compileTime The optional compile time of the program;
   * typically provided by the compiler-defined macro <b>__TIME__</b>.
   */
  Application(int argc, char** argv, const String& version = "0.0",
              const String& compileDate = "",
              const String& compileTime = "");

  /** Destructor. */
  virtual ~Application();

 protected:

  /**
   * Shutdown method. This method is called automatically when the
   * application is terminated via a keyboard interrupt (typically
   * Control-C). Subclasses may provide application-specific cleanup
   * logic in this method. Since this method gets called
   * from a signal handler, it should only perform minimal
   * processing. It is not necessary for this method to call exit(),
   * as that will be done automatically by the Application class.
   *
   * The default implementation does nothing.
   */
  virtual void shutdown();

  /**
   * Register a command-line option. The option may have a short
   * (one character) form and/or a long form. A typical invocation might
   * look like this:
   *
   * <code>registerOption('o', "output", "file", "Specify @@ as the output file.");</code>
   *
   * The corresponding usage help text for this switch would appear as:
   * <pre>
   * <b>-o</b> &lt;file&gt;
   * <b>--output=</b>&lt;file&gt;    - Specify &lt;file&gt; as the output file.
   * </pre>
   * @param opt The one-character switch (e.g., 'v' for "-v"), or <b>NUL</b>
   * if not applicable.
   * @param longOpt The long name for the switch (e.g., "verbose" for
   * "--verbose"), or <b>String::null</b> if not applicable.
   * @param argName A token describing the argument for the switch, or the
   * empty string "" if the switch does not accept an argument.
   * @param desc The description of the option, used in generating the usage
   * text. Any occurrence of the token "@@" within the description will be
   * replaced with a stylized form of the argument name, either by
   * underlining the token or by enclosing it within angle brackets.
   * @return <b>true</b> if the option was registered successfully,
   * <b>false</b> otherwise. Attempts to register an existing option will
   * fail.
   */
  bool registerOption(char opt, const String& longOpt,
                      const String& argName, const String& desc);

  /**
   * Parse the command line options. This method should be called from
   * the constructor at the appropriate time, after any application-specific
   * options have been registered and any other necessary initialization has
   * been completed.
   *
   * @param argc The argument count.
   * @param argv The argument list.
   * @param index Upon return, the index of the first non-option argument.
   * @return <b>true</b> if the options were parsed successfully, or
   * <b>false</b> if there was a parsing error. (Parsing errors are reported
   * to stderr as a side-effect.)
   */
  bool parseOptions(int argc, char** argv, int& index);

  /**
   * %Process a command-line option. This method is called for each
   * registered option that is encountered on the command line
   * during the call to <b>parseOptions</b>().
   * <p>
   * The <b>Application</b> class registers the "--help" and
   * "--version" command line options automatically. Therefore a
   * subclass should override <b>processOption</b>() such that if it
   * does not recognize the option, it will call
   * <b>Application::processOption</b>() to give it a chance to
   * process it.
   *
   * @param opt The one-character switch, or <b>NUL</b> if not applicable.
   * @param longOpt The long name for the switch, or <b>String::null</b> if
   * not applicable.
   * @param arg The argument for the switch, or <b>NULL</b> if not
   * applicable.
   * @return <b>true</b> if the option was processed successfully,
   * <b>false</b> otherwise.
   */
  virtual bool processOption(char opt, const String& longOpt,
                             const String& arg = String::null);

  /** Print the usage text. */
  virtual void printUsage();

  /**
   * Print an error message. The program name is automatically
   * prepended to the message.
   */
  void printError(const char* message, ...);

  /** @cond INTERNAL */
  String _version;
  String _compileDate;
  String _compileTime;
  CommandOptionList* _options;
  int _numLongOpts;
  /** @endcond */

 private:

  void buildUsageText();
  void doShutdown();

  bool _argsParsed;
  bool _useStyles;
  String _helpText;
  String _optString;

  static Application* _instance;

  static void _sighandler(int sig);
};

} // namespace ccxx

#endif // __ccxx_Application_hxx
