#include <SFML/Graphics.hpp>
#include <filesystem>
namespace fs = std::filesystem;
#include <iostream>
#include <vector>

#include "config.hpp"
#include "cxxopts.hpp"

// contains sprite and other useful image data
class Image {
 private:
  sf::Texture tex;
  std::string path;

 public:
  sf::Sprite sprite;
  bool exists = false;
  float width, height;

  Image(std::string filepath) {
    path = filepath;
    if (!tex.loadFromFile(path)) return;
    exists = true;

    width = tex.getSize().x;
    height = tex.getSize().y;
    tex.setSmooth(true);
    sprite.setTexture(tex);
  }
};

// pollute the namespace
unsigned int indx = 0;
sf::RenderWindow win;
std::vector<Image*> imgs;
bool fullscreen;
char mode;

// prototypes
void draw_single();
void toggle_fullscreen();

int main(int argc, char** argv) {
  std::string path_str;
  fs::path path;

  // configure command line options
  cxxopts::Options options("zamiv", "ZAMIV's A Mediocre Image Viewer");

  // clang-format off
  options.add_options()
    ("f,fullscreen", "Start in fullscreen")
    ("g,gallery", "Start in gallery mode")
    ("h,help", "Display usage")
    ("m", "Set scaling mode: [n]one, [f]it, [z]oom",
     cxxopts::value<std::string>()->default_value(MODE), "MODE")
    ("path", "", cxxopts::value<std::string>())
    ;
  // clang-format on

  options.parse_positional({"path"});
  options.custom_help("[-fhg] [-m MODE]");
  options.positional_help("PATH");
  options.allow_unrecognised_options();
  auto result = options.parse(argc, argv);

  // check fatal options
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  if (!result.count("path")) {
    std::cout << "error: must specify PATH" << std::endl;
    std::cout << options.help() << std::endl;
    exit(1);
  }
  path_str = result["path"].as<std::string>();

  path = path_str;
  if (!fs::exists(path)) {
    std::cout << "error: PATH doesn't exist" << std::endl;
    exit(1);
  }

  // set non-fatal options
  fullscreen = !result.count("fullscreen");

  mode = result["m"].as<std::string>().at(0);
  if (mode != 'n' && mode != 'f' && mode != 'z') {
    std::cout << "warning: invalid MODE, defaulting to 'n'" << std::endl;
    mode = 'n';
  }

  if (fs::is_directory(path))
    for (const auto& entry : fs::directory_iterator(path_str)) {
      Image* img = new Image(entry.path());
      if (img->exists)
        imgs.push_back(img);
      else
        delete img;
    }
  else
    imgs.push_back(new Image(path_str));

  if (!imgs.size()) {
    std::cout << "error: no images to display" << std::endl;
    exit(1);
  }

  // create window
  toggle_fullscreen();
  win.setVerticalSyncEnabled(true);

  while (win.isOpen()) {
    sf::Event event;
    while (win.pollEvent(event)) {
      switch (event.type) {
        // key bindings
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::Q:
              win.close();
              break;
            case sf::Keyboard::F:
              toggle_fullscreen();
              break;
            case sf::Keyboard::N:
              if (indx < imgs.size() - 1) indx++;
              break;
            case sf::Keyboard::P:
              if (indx > 0) indx--;
              break;
            default:
              break;
          }
          break;
        // update render area to window dimensions on resize
        case sf::Event::Resized: {
          sf::FloatRect view(0, 0, event.size.width, event.size.height);
          win.setView(sf::View(view));
        }
        default:
          break;
      }
    }

    // drawing
    win.clear(bg_color);
    draw_single();
    win.display();
  }

  return 0;
}

// draw single image
void draw_single() {
  float win_width, win_height;
  float width_ratio, height_ratio;
  Image* pimg;
  float x_scale = 1, y_scale = 1;
  float x, y;

  win_width = win.getView().getSize().x;
  win_height = win.getView().getSize().y;
  pimg = imgs.at(indx);
  width_ratio = win_width / pimg->width;
  height_ratio = win_height / pimg->height;

  // set scale factors depending on scaling mode
  switch (mode) {
    case 'n':
      if (!(pimg->width > win_width || pimg->height > win_height)) break;
      // fall through
    case 'f':
      x_scale = (width_ratio > height_ratio) ? height_ratio : width_ratio;
      y_scale = x_scale;
      break;
    case 'z':
      x_scale = (width_ratio < height_ratio) ? height_ratio : width_ratio;
      y_scale = x_scale;
  }

  pimg->sprite.setScale(x_scale, y_scale);

  x = (win_width - pimg->width * x_scale) / 2;
  y = (win_height - pimg->height * x_scale) / 2;

  pimg->sprite.setPosition(x, y);
  win.draw(pimg->sprite);
}

// create window, fullscreen or otherwise
void toggle_fullscreen() {
  if (fullscreen) {
    fullscreen = false;
    win.create(sf::VideoMode(WIDTH, HEIGHT), "zamiv");
  } else {
    int width = sf::VideoMode::getDesktopMode().width;
    int height = sf::VideoMode::getDesktopMode().height;

    fullscreen = true;
    win.create(sf::VideoMode(width, height), "zamiv", sf::Style::Fullscreen);
  }
}
