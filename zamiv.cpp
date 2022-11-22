#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <filesystem>
namespace fs = std::filesystem;
#include <iostream>
#include <vector>

#include "config.hpp"
#include "cxxopts.hpp"

class Image {
 private:
  sf::Texture tex;

 public:
  std::string path;
  sf::Sprite sprite;
  bool exists = false;

  Image(std::string filepath) {
    path = filepath;
    if (!tex.loadFromFile(path)) return;
    exists = true;

    sprite.setTexture(tex);
  }
};

// pollute the namespace
int indx = 0;
sf::RenderWindow win;
std::vector<Image*> imgs;
bool fullscreen;

void draw_single(std::vector<Image*>*);
void toggle_fullscreen(bool* fullscreen);

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
     cxxopts::value<char>()->default_value("n"), "MODE")
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

  if (fs::is_directory(path))
    for (const auto& entry : fs::directory_iterator(path_str)) {
      Image* img = new Image(entry.path());
      if (img->exists) imgs.push_back(img);
      else delete img;
    }
  else
    imgs.push_back(new Image(path_str));

  if (!imgs.size()) {
    std::cout << "error: no images to display" << std::endl;
    exit(1);
  }

  // create window
  toggle_fullscreen(&fullscreen);
  win.setVerticalSyncEnabled(true);

  while (win.isOpen()) {
    sf::Event event;
    while (win.pollEvent(event)) {
      // key bindings
      switch (event.type) {
        case sf::Event::KeyPressed:
          if (event.key.code == sf::Keyboard::Q)
            win.close();
          else if (event.key.code == sf::Keyboard::F)
            toggle_fullscreen(&fullscreen);
          break;
        default:
          break;
      }
    }

    // drawing
    win.clear(bg_color);
    draw_single(&imgs);
    win.display();
  }

  return 0;
}

// draw single image
void draw_single(std::vector<Image*>* imgs) {
  int xc = sf::VideoMode::getDesktopMode().width / 2;
  int yc = sf::VideoMode::getDesktopMode().height / 2;
  sf::Sprite* spref = &(imgs->at(indx)->sprite);
  sf::FloatRect e_rect = spref->getGlobalBounds();
  int x = xc - e_rect.width / 2;
  int y = yc - e_rect.height / 2;

  spref->setPosition(x, y);
  win.draw(*spref);
}

// create window, fullscreen or otherwise
void toggle_fullscreen(bool* fullscreen) {
  if (*fullscreen) {
    *fullscreen = false;
    win.create(sf::VideoMode(WIDTH, HEIGHT), "zamiv");
  } else {
    int width = sf::VideoMode::getDesktopMode().width;
    int height = sf::VideoMode::getDesktopMode().height;

    *fullscreen = true;
    win.create(sf::VideoMode(width, height), "zamiv", sf::Style::Fullscreen);
  }
}
