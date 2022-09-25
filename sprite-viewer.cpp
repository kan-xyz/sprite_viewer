#include "sprite-viewer.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"
#include <cmath>
#include <fstream>
#include <sstream>

namespace arcsandbox {

	struct SpriteViewerConfigData {

		std::string fontFile;
		unsigned int characterSize = 20;
		sf::Vector2f textPadding;

		std::string textureFile;
		sf::Vector2f viewRectPos, viewRectSize;
		sf::Vector2f viewRectMoveSpeed, viewRectMoveSpeedSlow;
		sf::Vector2f viewRectResizeSpeed, viewRectResizeSpeedSlow;

		sf::Vector2f viewMoveSpeed;
		float zoomSpeed = 1.0f;

	};

	bool loadConfig(SpriteViewerConfigData& config) {

		std::ifstream file;
		file.open("spv_config.txt");
		if (!file.is_open()) {
			printf("\n[ERROR]: could not load config file sprite-viewer-config.txt\n");
			return false;
		}

		std::string buffer;
		while (std::getline(file, buffer)) {

			if (buffer.empty()) { continue; }
			else if (buffer[0] == '|') { continue; }

			std::stringstream ss(buffer);
			std::string type;
			ss >> type;

			if (type == "FONT") {
				config.fontFile = buffer;
				config.fontFile.erase(0, type.size() + 1);
			}

			else if (type == "FONT_SIZE") {
				std::string s;
				ss >> s;
				config.characterSize = (unsigned int)std::stoi(s);
			}

			else if (type == "TEXT_PADDING") {
				std::string x, y;
				ss >> x >> y;
				config.textPadding = { std::stof(x), std::stof(y) };
			}

			else if (type == "TEXTURE") {
				config.textureFile = buffer;
				config.textureFile.erase(0, type.size() + 1);
			}

			else if (type == "VIEW_RECT_SIZE") {
				std::string x, y, w, h;
				ss >> x >> y >> w >> h;
				config.viewRectSize = { std::stof(w), std::stof(h) };
				config.viewRectPos = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_RECT_SPEED") {
				std::string x, y;
				ss >> x >> y;
				config.viewRectMoveSpeed = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_RECT_SPEED_SLOW") {
				std::string x, y;
				ss >> x >> y;
				config.viewRectMoveSpeedSlow = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_RECT_RESIZE_SPEED") {
				std::string x, y;
				ss >> x >> y;
				config.viewRectResizeSpeed = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_RECT_RESIZE_SPEED_SLOW") {
				std::string x, y;
				ss >> x >> y;
				config.viewRectResizeSpeedSlow = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_MOVE_SPEED") {
				std::string x, y;
				ss >> x >> y;
				config.viewMoveSpeed = { std::stof(x), std::stof(y) };
			}

			else if (type == "VIEW_ZOOM_SPEED") {
				std::string z;
				ss >> z;
				config.zoomSpeed = std::stof(z);
			}

		}

		file.close();
		return true;
	}

	struct SpriteViewerData {

		sf::RectangleShape viewRect, textureRect, sprite, infoBox, instructionBox;
		sf::Text x, y, w, h, info, instructions, notice;
		sf::Texture texture;
		sf::Font font;
		sf::View view;

	};

	bool loadSpriteViewerData(SpriteViewerConfigData& config, SpriteViewerData& data, sf::RenderWindow& window) {

		if (!data.texture.loadFromFile(config.textureFile)) {
			printf("\n[ERROR]: sfml could not load texture %s\n", config.textureFile.c_str());
			return false;
		}

		const float outlineThickness = 5.0f;

		data.sprite.setSize(sf::Vector2f(data.texture.getSize()));
		data.sprite.setFillColor(sf::Color::White);
		data.sprite.setOutlineColor(sf::Color::Green);
		data.sprite.setOutlineThickness(outlineThickness);
		data.sprite.setTexture(&data.texture);

		const sf::Vector2f wSize = sf::Vector2f(window.getSize());

		data.textureRect.setSize({ 300.0f, 300.0f });
		data.textureRect.setPosition({ wSize.x - data.textureRect.getSize().x - outlineThickness, outlineThickness });
		data.textureRect.setTexture(&data.texture);
		data.textureRect.setFillColor(sf::Color::White);
		data.textureRect.setOutlineColor(sf::Color::Green);
		data.textureRect.setOutlineThickness(outlineThickness);

		data.viewRect.setPosition(config.viewRectPos);
		data.viewRect.setSize(config.viewRectSize);
		data.viewRect.setFillColor({ 0, 0, 0, 0 });
		data.viewRect.setOutlineColor(sf::Color::Red);
		data.viewRect.setOutlineThickness(outlineThickness);

		data.infoBox.setPosition(data.textureRect.getPosition());
		data.infoBox.setSize({ data.textureRect.getSize().x, wSize.y - 2.0f * outlineThickness });
		data.infoBox.setFillColor(sf::Color::Black);
		data.infoBox.setOutlineColor(sf::Color::Green);
		data.infoBox.setOutlineThickness(outlineThickness);

		if (!data.font.loadFromFile(config.fontFile)) {
			printf("\n[ERROR]: could not load font %s\n", config.fontFile.c_str());
			return false;
		}

		data.x.setFont(data.font);
		data.y.setFont(data.font);
		data.w.setFont(data.font);
		data.h.setFont(data.font);
		data.info.setFont(data.font);
		data.instructions.setFont(data.font);
		data.notice.setFont(data.font);

		data.x.setCharacterSize(config.characterSize);
		data.y.setCharacterSize(config.characterSize);
		data.w.setCharacterSize(config.characterSize);
		data.h.setCharacterSize(config.characterSize);
		data.info.setCharacterSize(config.characterSize);
		data.instructions.setCharacterSize(config.characterSize);
		data.notice.setCharacterSize(config.characterSize);

		data.x.setFillColor(sf::Color::White);
		data.y.setFillColor(sf::Color::White);
		data.w.setFillColor(sf::Color::White);
		data.h.setFillColor(sf::Color::White);
		data.info.setFillColor(sf::Color::White);
		data.instructions.setFillColor(sf::Color::White);
		data.notice.setFillColor(sf::Color::White);

		sf::Vector2f textPos = {
			data.textureRect.getPosition().x + config.textPadding.x,
			data.textureRect.getPosition().y + data.textureRect.getSize().y + 30.0f
		};

		data.x.setPosition(textPos);
		textPos.y += config.textPadding.y;

		data.y.setPosition(textPos);
		textPos.y += config.textPadding.y;

		data.w.setPosition(textPos);
		textPos.y += config.textPadding.y;

		data.h.setPosition(textPos);
		textPos.y += 2.0f * config.textPadding.y;

		data.info.setPosition(textPos);
		textPos.y += config.textPadding.y;

		data.instructions.setPosition(textPos);
		textPos.y += 4.0f * config.textPadding.y;

		data.notice.setPosition(textPos);

		data.x.setString("x = " + std::to_string(0));
		data.y.setString("y = " + std::to_string(0));
		data.w.setString("w = " + std::to_string(0));
		data.h.setString("h = " + std::to_string(0));
		data.info.setString("movement:");
		data.instructions.setString("press arrow keys to\nmove the view rect");
		data.notice.setString("use the number keys 1 to 7\nto change the mode");

		return true;
	}

	enum class Mode {

		MoveRect,
		MoveRectSlow,
		ResizeRect,
		ResizeRectSlow,
		MoveView,
		ZoomView,
		MoveFrame

	};

	void moveViewRect(sf::RectangleShape& viewRect, const sf::Time& timestep, const sf::Vector2f& velocity) {

		sf::Vector2f v;
		const float dt = timestep.asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { v.y -= dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { v.y += dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { v.x -= dt * velocity.x; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { v.x += dt * velocity.x; }

		viewRect.move(v);

	}

	void resizeViewRect(sf::RectangleShape& viewRect, const sf::Time& timestep, const sf::Vector2f& velocity) {

		sf::Vector2f v;
		const float dt = timestep.asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { v.y -= dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { v.y += dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { v.x -= dt * velocity.x; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { v.x += dt * velocity.x; }

		viewRect.setSize(viewRect.getSize() + v);

	}

	void moveView(sf::View& view, const sf::Time& timestep, const sf::Vector2f& velocity) {

		sf::Vector2f v;
		const float dt = timestep.asSeconds();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { v.y -= dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { v.y += dt * velocity.y; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { v.x -= dt * velocity.x; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { v.x += dt * velocity.x; }

		view.move(v);

	}

	void zoomView(sf::View& view, const float zoom) {

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			view.zoom(1.0f / zoom);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			view.zoom(zoom);
		}

	}

	void moveFrame(sf::RectangleShape& viewRect) {

		const sf::Vector2f& p = viewRect.getPosition();
		const sf::Vector2f& s = viewRect.getSize();

		viewRect.setPosition({ std::floor(p.x), std::floor(p.y) });
		viewRect.setSize({ std::floor(s.x), std::floor(s.y) });

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			viewRect.setPosition({ p.x, p.y - s.y });
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			viewRect.setPosition({ p.x, p.y + s.y });
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			viewRect.setPosition({ p.x - s.x, p.y });
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			viewRect.setPosition({ p.x + s.x, p.y });
		}

	}

	void clampRect(sf::RectangleShape& viewRect, const sf::Vector2f& size) {

		// clamp position
		{
			sf::Vector2f p = viewRect.getPosition();
			sf::Vector2f s = viewRect.getSize();

			if (p.x + s.x > size.x) {
				viewRect.setPosition({ size.x - s.x, p.y });
			}
			else if (p.x < 0.0f) {
				viewRect.setPosition({ 0.0f, p.y });
			}

			// this is here because of a bug involving the out of bounds
			// view rect
			p = viewRect.getPosition();
			s = viewRect.getSize();

			if (p.y + s.y > size.y) {
				viewRect.setPosition({ p.x, size.y - s.y });
			}
			else if (p.y < 0.0f) {
				viewRect.setPosition({ p.x, 0.0f });
			}
		}

		// clamp size
		{
			const sf::Vector2f p = viewRect.getPosition();
			const sf::Vector2f s = viewRect.getSize();

			if (p.x + s.x > size.x) {
				viewRect.setSize({ size.x - p.x, s.y });
			}
			else if (s.x < 0.0f) {
				viewRect.setSize({ 0.0f, s.y });
			}

			if (p.y + s.y > size.y) {
				viewRect.setSize({ s.x, size.y - p.y });
			}
			else if (p.y < 0.0f) {
				viewRect.setSize({ s.x, 0.0f });
			}
		}

	}

	void updateInfo(SpriteViewerData& data) {

		const sf::Vector2f p = data.viewRect.getPosition();
		const sf::Vector2f s = data.viewRect.getSize();
		const sf::FloatRect fr = { std::floor(p.x), std::floor(p.y), std::floor(s.x), std::floor(s.y) };
		const sf::IntRect area = sf::IntRect(fr);
		data.textureRect.setTextureRect(area);

		data.x.setString("x = " + std::to_string(area.left));
		data.y.setString("y = " + std::to_string(area.top));
		data.w.setString("w = " + std::to_string(area.width));
		data.h.setString("h = " + std::to_string(area.height));

	}

	void spriteViewer() {

		sf::RenderWindow window({ 1280, 720 }, "sprite viewer");
		window.setFramerateLimit(30);

		SpriteViewerConfigData config;
		SpriteViewerData data;
		Mode mode = Mode::MoveRect;

		if (!loadConfig(config)) { return; }
		if (!loadSpriteViewerData(config, data, window)) { return; }

		bool running = true;
		sf::Clock cl;

		while (running) {

			sf::Event e;
			while (window.pollEvent(e)) {

				if (e.type == sf::Event::Closed) { running = false; }

				if (e.type == sf::Event::KeyPressed) {

					if (e.key.code == sf::Keyboard::Num1) {
						mode = Mode::MoveRect;
						data.info.setString("movement:");
						data.instructions.setString("press arrow keys to\nmove the view rect");
					}

					else if (e.key.code == sf::Keyboard::Num2) {
						mode = Mode::MoveRectSlow;
						data.info.setString("slow movement:");
						data.instructions.setString("press arrow keys to\nmove the view rect");
					}
					
					else if (e.key.code == sf::Keyboard::Num3) {
						mode = Mode::ResizeRect;
						data.info.setString("resize:");
						data.instructions.setString("press arrow keys to\nresize the view rect");
					}
					
					else if (e.key.code == sf::Keyboard::Num4) {
						mode = Mode::ResizeRectSlow;
						data.info.setString("slow resize:");
						data.instructions.setString("press arrow keys to\nmove the view rect");
					}
					
					else if (e.key.code == sf::Keyboard::Num5) {
						mode = Mode::MoveView;
						data.info.setString("view movemwnt:");
						data.instructions.setString("press arrow keys to\nmove the camera around");
					}
					
					else if (e.key.code == sf::Keyboard::Num6) {
						mode = Mode::ZoomView;
						data.info.setString("zoom:");
						data.instructions.setString("press up and down keys to\nzoom the camera");
					}
					
					else if (e.key.code == sf::Keyboard::Num7) {
						mode = Mode::MoveFrame;
						data.info.setString("frame movement:");
						data.instructions.setString("press arrow keys to\nmove by one frame");
					}

					else if (e.key.code == sf::Keyboard::Space) {
						const sf::Vector2f pos = data.viewRect.getPosition();
						data.viewRect.setPosition({ std::floor(pos.x), std::floor(pos.y) });
					}

					if (mode == Mode::MoveFrame) {
						moveFrame(data.viewRect);
					}

				}

			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { running = false; }

			const sf::Time timestep = cl.restart();

			if (mode == Mode::MoveRect) {
				moveViewRect(data.viewRect, timestep, config.viewRectMoveSpeed);
			}

			else if (mode == Mode::MoveRectSlow) {
				moveViewRect(data.viewRect, timestep, config.viewRectMoveSpeedSlow);
			}

			else if (mode == Mode::ResizeRect) {
				resizeViewRect(data.viewRect, timestep, config.viewRectResizeSpeed);
			}

			else if (mode == Mode::ResizeRectSlow) {
				resizeViewRect(data.viewRect, timestep, config.viewRectResizeSpeedSlow);
			}

			else if (mode == Mode::MoveView) {
				moveView(data.view, timestep, config.viewMoveSpeed);
			}

			else if (mode == Mode::ZoomView) {
				zoomView(data.view, config.zoomSpeed);
			}

			clampRect(data.viewRect, data.sprite.getSize());
			updateInfo(data);

			window.clear({ 0, 30, 30, 255 });

			window.setView(data.view);
			window.draw(data.sprite);
			window.draw(data.viewRect);

			window.setView(window.getDefaultView());
			window.draw(data.infoBox);
			window.draw(data.textureRect);
			window.draw(data.x);
			window.draw(data.y);
			window.draw(data.w);
			window.draw(data.h);
			window.draw(data.info);
			window.draw(data.instructions);
			window.draw(data.notice);

			window.display();

		}

	}

}
