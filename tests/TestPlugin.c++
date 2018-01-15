
#include <commonc++/Plugin.h++>

namespace test_plugins {

class TestPlugin : public ccxx::Plugin
{
  public:

  TestPlugin()
  {
  }

  virtual ~TestPlugin() throw()
  {
  }
};

} // namespace test_plugins

CCXX_EXPORT_PLUGIN(test_plugins::TestPlugin, "TestPlugin", "1.0",
                   "someone@somewhere.com");

