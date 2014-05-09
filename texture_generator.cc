#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
void DisplayOptions()
{
  cout << "OPTIONS:" << endl;
  cout << "  -f <output filename [texture.svg]>" << endl;
  cout << "  -pw <page width (mm) [210]>" << endl;
  cout << "  -ph <page height (mm) [297]>" << endl;
  cout << "  -d <marker density (/m^2) [100000]>" << endl;
  cout << "  -mw <minimum marker width (mm) [2]>" << endl;
  cout << "  -bl <background lightness (0-255) [127]>" << endl;
  cout << endl;
}

//------------------------------------------------------------------------------
bool CmdOptionExists(char** begin, char** end, const string& option)
{
  return find(begin, end, option) != end;
}

//------------------------------------------------------------------------------
static char* CmdOption(char** begin, char** end, const string& option)
{
  char** itr = find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

//------------------------------------------------------------------------------
// Writes the beginning of the SVG file, including the gray background
static void SVGWriteHeader(ofstream& file, int width, int height,
  int background_lightness)
{
  file << "<?xml version=\"1.0\" standalone=\"no\"?>" << endl;
  file << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"" << endl;
  file << "    \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << endl;
  file << endl;
  file << "<svg width=\"" << width << "mm\" height=\"" << height
    << "mm\" viewBox=\"0 0 " << width << " " << height << "\"" << endl;
  file << "    xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << endl;
  file << endl;
  file << "  <title>University of Tokyo texture generator</title>" << endl;
  file << "  <desc>Generated texture for FAST corner detector</desc>" << endl;
  file << endl;
  file << "  <rect width=\"" << width << "\" height=\"" << height
    << "\" fill=\"#";
  for (int i = 0; i < 3; i++)
    file << hex << setw(2) << background_lightness;
  file << "\" />" << endl;
  file << endl;
}

//------------------------------------------------------------------------------
// Closes the svg tag
static void SVGWriteFooter(ofstream& file)
{
  file << "</svg>";
}

//------------------------------------------------------------------------------
// Add a plus to the SVG file given it's position, rotation, size and lightness
static void SVGAddPlus(ofstream& file, float x, float y, float rotation,
  float size, int lightness)
{
  rotation *= M_PI;

  file << "  <path d=\"M ";

  const float plus[12][2] =
  {
    {-.5, .5/3.}, {-.5/3., .5/3.}, {-.5/3., .5}, {.5/3., .5},
    {.5/3., .5/3.}, {.5, .5/3.}, {.5, -.5/3.}, {.5/3., -.5/3.},
    {.5/3., -.5}, {-.5/3., -.5}, {-.5/3., -.5/3.}, {-.5, -.5/3.}
  };

  for (int i = 0; i < 12; ++i)
  {
    file
      << (plus[i][0] * cos(rotation) - plus[i][1] * sin(rotation)) * size + x
      << ","
      << (plus[i][0] * sin(rotation) + plus[i][1] * cos(rotation)) * size + y
      << " ";
  }

  file << "z\"" << endl;

  file << "    fill=\"#";
  for (int i = 0; i < 3; i++)
    file << hex << setw(2) << lightness;
  file << "\" />" << endl;
  file << endl;
}

//------------------------------------------------------------------------------
// Uniform pseudo-random number from "lower_limit" to "upper_limit"
static float RandUni(float lower_limit, float upper_limit)
{
  return (float)((double)rand() / (double)RAND_MAX)
    * (upper_limit - lower_limit) + lower_limit;
}

//------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
  char default_filename[] = "texture.svg";

  // Other default values (can be overridden with command line options):
  float width = 210., height = 297.;
  float density = 10000.;  // Objects per square m
  float minimum_size = 2.;  // Millimeters
  int background_lightness = 127;

  cout << endl;
  cout << "University of Tokyo texture generator for FAST corner detector"
    << endl;
  cout << "--------------------------------------------------------------"
    << endl;
  cout << endl;

  ofstream output_file;

  // Parse command line options
  if (CmdOptionExists(argv, argv + argc, "-h"))
  {
    DisplayOptions();
    return 0;
  }
  else
  {
    cout << "Use -h to see extra options." << endl;
    cout << endl;
  }

  char* output_filename = CmdOption(argv, argv + argc, "-f");
  if (!output_filename)
    output_filename = default_filename;

  char* width_ascii = CmdOption(argv, argv + argc, "-pw");
  if (width_ascii)
    width = atof(width_ascii);

  char* height_ascii = CmdOption(argv, argv + argc, "-ph");
  if (height_ascii)
    height = atof(height_ascii);

  char* density_ascii = CmdOption(argv, argv + argc, "-d");
  if (density_ascii)
    density = atof(density_ascii);

  char* minimum_size_ascii = CmdOption(argv, argv + argc, "-mw");
  if (minimum_size_ascii)
    minimum_size = atof(minimum_size_ascii);

  char* background_lightness_ascii = CmdOption(argv, argv + argc, "-bl");
  if (background_lightness_ascii)
    background_lightness = atoi(background_lightness_ascii);

  // Open the output file
  output_file.open(output_filename);
  if (!output_file.is_open())
  {
    cout << "Unable to open " << output_filename << " for output" << endl;
    return 1;
  }

  // Set file output stream to append 0's on the front of numbers where width is
  // specified (in this case, used for specifying hex lightness values.)
  output_file << internal << setfill('0');

  SVGWriteHeader(output_file, width, height, background_lightness);

  // Initialize the random seed:
  srand(time(NULL));

  // Start generating pluses
  const int number = (int)(density * (width / 1000.) * (height / 1000.) + .5);
  const float maximum_size = .8 * min(width, height);
  const float density_max = 1. / maximum_size / maximum_size;  // relative
  const float density_min = 1. / minimum_size / minimum_size;  // relative
  const int lightness_gap = 40;
  for (int i = 0; i < number; ++i)
  {
    float size = sqrt(1. / (((float)i / (float)(number - 1))
      * (density_min - density_max) + density_max));
    float half_size = size / 2.;
    float x = RandUni(half_size, width - half_size);
    float y = RandUni(half_size, height - half_size);
    float rotation = RandUni(-.25, .25);
    int lightness = rand() % (255 - lightness_gap);
    if (lightness > background_lightness - lightness_gap / 2)
      lightness += lightness_gap;

    SVGAddPlus(output_file, x, y, rotation, size, lightness);
  }

  SVGWriteFooter(output_file);

  output_file.close();

  cout << "DONE: Drew " << number << " pluses in file " << output_filename << endl;
  cout << endl;

  return 0;
}