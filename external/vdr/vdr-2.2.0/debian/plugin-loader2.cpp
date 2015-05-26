#include <dirent.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm> 
#include <cctype>
#include <fstream> 
#include <functional> 
#include <iostream>
#include <locale>
#include <vector>

using namespace std;

// trim from start
static inline string &ltrim(string &s)
{
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  return s;
}

// trim from end
static inline string &rtrim(string &s)
{
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

// trim from both ends
static inline string &trim(string &s)
{
  return ltrim(rtrim(s));
}

static int IndexOf(const vector<string> &list, const string &entry)
{
  int index = 0;
  for (vector<string>::const_iterator it = list.begin(); it != list.end(); ++it) {
      if (*it == entry)
         return index;
      ++index;
      }
  return -1;
}

// read file line by line, drop empty lines and everything after #
static vector<string> &ReadFile(vector<string> &list, const string &filename)
{
  string line;
  size_t pos;
  list.clear();
  try
  {
    ifstream file(filename.c_str(), ifstream::in);
    while (file.peek() != EOF) {
          getline(file, line);
          line = trim(line);
          pos = line.find("#");
          if (pos != string::npos)
             line = line.substr(0, pos);
          if (!line.empty())
             list.push_back(line);
          }
    file.close();
  }
  catch (...)
  {
  }
  return list;
}

static void ReadOrderConf(vector<string> &top, vector<string> &bottom, vector<string> &disabled, const string &filename)
{
  vector<string> lines;
  top.clear();
  bottom.clear();
  disabled.clear();
  ReadFile(lines, filename);
  bool in_top = true;
  for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it) {
      if ((*it).substr(0, 1) == "*") {
         if ((*it).length() == 1) // new order.conf syntax with an * spliting first and last plugins
            in_top = false;
         else // old yavdr order.conf syntax with an * in front of all last plugins
            bottom.push_back((*it).substr( 1, (*it).length()));
         }
      else if ((*it).substr(0, 1) == "-")
         disabled.push_back((*it).substr(1, string::npos));
      else if (in_top)
         top.push_back(*it);
      else
         bottom.push_back(*it);
      }
}

static string &GetPluginParameters(string &parameters, const string &plugin_cfg_dir, const string &plugin)
{
  vector<string> lines;
  parameters.clear();
  ReadFile(lines, plugin_cfg_dir + "/plugin." + plugin + ".conf");
  for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it)
      parameters += " " + *it;
  if (parameters.empty())
     parameters = " -P " + plugin;
  else
     parameters = " -P \"" + plugin + parameters + "\"";
  return parameters;
}

static bool HasPluginParameter(const string &plugin_cfg_dir, const string &plugin, const string &parameter_name)
{
  vector<string> lines;
  ReadFile(lines, plugin_cfg_dir + "/plugin." + plugin + ".conf");
  for (vector<string>::iterator it = lines.begin(); it != lines.end(); ++it) {
      if ((*it).find(parameter_name) != string::npos)
         return true;
      }
  return false;
}

static vector<string> &GetInstalledPlugins(vector<string> &plugins, const string &plugin_dir, const string &vdr_apiversion, const vector<string> &disabled)
{
  plugins.clear();
  DIR *dir = opendir(plugin_dir.c_str());
  if (dir == NULL)
     return plugins;
  struct dirent *ent;
  struct stat ds;
  string name;
  string plugin;
  string praefix = "libvdr-";
  string ext = ".so." + vdr_apiversion;
  while ((ent = readdir(dir)) != NULL) {
        name = ent->d_name;
        if (name.length() <= (praefix.length() + ext.length()))
        if ((stat(name.c_str(), &ds) != 0) || !S_ISREG(ds.st_mode))
           continue;
        if ((name.substr(0, praefix.length()) != praefix) || (name.substr(name.length() - ext.length(), ext.length()) != ext))
           continue;
        plugin = name.substr(praefix.length(), name.length() - praefix.length() - ext.length());
        if (IndexOf(disabled, plugin) < 0)
           plugins.push_back(plugin);
        }
  closedir(dir);
  return plugins;
}

int main(int argc, char *argv[])
{
  // config
  string vdr_apiversion;
  string plugin_cfg_dir;
  string plugin_dir;
  string plugin;
  string has_parameter;

  // read parameter from environment
  const char *tmp = getenv("VDR_APIVERSION");
  if (tmp != NULL)
     vdr_apiversion = tmp;

  plugin_cfg_dir = "/etc/vdr/plugins";
  tmp = getenv("PLUGIN_CFG_DIR");
  if (tmp != NULL)
     plugin_cfg_dir = tmp;

  plugin_dir = "/usr/lib/vdr/plugins";
  tmp = getenv("PLUGIN_DIR");
  if (tmp != NULL)
     plugin_dir = tmp;

  static struct option long_options[] = {
      { "apiversion",    required_argument, NULL, 'a' },
      { "confdir",       required_argument, NULL, 'c' },
      { "libdir",        required_argument, NULL, 'L' },
      { "plugin",        required_argument, NULL, 'P' },
      { "has-parameter", required_argument, NULL, 'h' },
      { NULL,            no_argument,       NULL,  0  }
    };

  int c;
  while ((c = getopt_long(argc, argv, "a:c:L:P:h:", long_options, NULL)) != -1) {
        switch (c) {
          case 'a':
            vdr_apiversion = optarg;
            break;
          case 'c':
            plugin_cfg_dir = optarg;
            break;
          case 'L':
            plugin_dir = optarg;
            break;
          case 'P':
            plugin = optarg;
            break;
          case 'h':
            has_parameter = optarg;
            break;
          }
        }

  if (!plugin.empty() && !has_parameter.empty()) {
     if (HasPluginParameter(plugin_cfg_dir, plugin, has_parameter)) {
        cout << "1";
        return 0;
        }
     return 1;
     }

  if (vdr_apiversion.empty()) {
     cerr << "missing VDR_APIVERSION" << endl;
     return 1;
     }

  vector<string> top;
  vector<string> bottom;
  vector<string> disabled;
  ReadOrderConf(top, bottom, disabled, plugin_cfg_dir + "/order.conf");

  vector<string> plugins;
  GetInstalledPlugins(plugins, plugin_dir, vdr_apiversion, disabled);
  string output_top;
  string parameters;
  for (vector<string>::iterator it = top.begin(); it != top.end(); ++it) {
      int pos = IndexOf(plugins, *it);
      if (pos >= 0) {
         output_top += GetPluginParameters(parameters, plugin_cfg_dir, *it);
         plugins.erase(plugins.begin() + pos);
         }
      }

  string output_bottom;
  for (vector<string>::iterator it = bottom.begin(); it != bottom.end(); ++it) {
      int pos = IndexOf(plugins, *it);
      if (pos >= 0) {
         output_bottom += GetPluginParameters(parameters, plugin_cfg_dir, *it);
         plugins.erase(plugins.begin() + pos);
         }
      }

  string output_center;
  for (vector<string>::iterator it = plugins.begin(); it != plugins.end(); ++it)
      output_center += GetPluginParameters(parameters, plugin_cfg_dir, *it);
      
  cout <<  output_top << output_center << output_bottom;
  return 0;
}
