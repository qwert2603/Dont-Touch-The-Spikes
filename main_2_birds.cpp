#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <utility>
#include <SFML\Graphics.hpp>
#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include "dont_touch_the_spikes.h"
namespace dtts = dont_touch_the_spikes;

// размеры окна
const unsigned width = 480;
const unsigned height = 640;
// радиус птиц
const double bird_radius = 24;
// сложность
const unsigned complexity = 4;
// параметры движения птиц
const double hor_speed = 280;
const double swing_speed = 420;
const double speed_up = 650;
// размеры шипов на боковых стенках
const double hor_spike_base = 32;
const double hor_spike_height = 32;
// размеры шипов на полу и потолке
const double ver_spike_base = 16;
const double ver_spike_height = 16;
// начальный радиус кружочка после взмаха
const double circle_radius = 12;
// кол-во кружочков после каждого взмаза
const unsigned circle_count = 7;
// время отображения кружочка
const double circle_duration = 1.0;
// шаг по времени
const double time_step = 0.015;

std::pair<dtts::BirdState, dtts::BirdState> make_objects(dtts::Field &_field) {
	// добавление птиц
	_field.add_bird(width / 2 - 50.0, height / 2, bird_radius, -1.0 * hor_speed, swing_speed, speed_up);
	_field.add_bird(width / 2 + 50.0, height / 2, bird_radius, hor_speed, swing_speed, speed_up);
	// добавить шипы на обе боковые стены
	_field.clear_hor_spikes();
	_field.add_random_hor_spikes(true);
	_field.add_random_hor_spikes(false);
	// включим кружочки
	_field.clear_circles(0);
	_field.clear_circles(1);
	_field.add_circles(0, circle_radius, circle_count, circle_duration);
	_field.add_circles(1, circle_radius, circle_count, circle_duration);
	// состояния птиц
	auto birds = _field.get_birds();
	return std::make_pair(birds[0], birds[1]);
}

// рисует заданную птицу в нужном месте в переданном окне
void draw_bird(const dtts::BirdState &_bird_state, sf::Sprite &_sprite, sf::RenderWindow &_window) {
	_sprite.setPosition(_bird_state.x - bird_radius, height - (_bird_state.y + bird_radius));
	_sprite.setTextureRect(sf::IntRect(_bird_state.right_direction ? 0 : 2 * bird_radius, 0, bird_radius * 2, bird_radius * 2));
	_window.draw(_sprite);
}

int main() {	
	// само окно
	sf::RenderWindow window(sf::VideoMode(width, height), "_dtts");

	// для картинки 0 птицы
	sf::Image bird_image_0;
	bird_image_0.loadFromFile("images/bird_0.png");
	bird_image_0.createMaskFromColor(sf::Color::White);
	sf::Texture bird_texture_0;
	bird_texture_0.loadFromImage(bird_image_0);
	sf::Sprite bird_sprite_0(bird_texture_0);

	// для картинки 1 птицы
	sf::Image bird_image_1;
	bird_image_1.loadFromFile("images/bird_1.png");
	bird_image_1.createMaskFromColor(sf::Color::White);
	sf::Texture bird_texture_1;
	bird_texture_1.loadFromImage(bird_image_1);
	sf::Sprite bird_sprite_1(bird_texture_1);

	// для картинки мертвой птицы
	sf::Image dead_bird_image;
	dead_bird_image.loadFromFile("images/dead_bird.png");
	dead_bird_image.createMaskFromColor(sf::Color::White);
	sf::Texture dead_bird_texture;
	dead_bird_texture.loadFromImage(dead_bird_image);
	sf::Sprite dead_bird_sprite(dead_bird_texture);

	// для картинки горизонтального шипа
	sf::Image hor_spike_image;
	hor_spike_image.loadFromFile("images/hor_spike.png");
	hor_spike_image.createMaskFromColor(sf::Color::White);
	sf::Texture hor_spike_texture;
	hor_spike_texture.loadFromImage(hor_spike_image);
	sf::Sprite hor_spike_sprite(hor_spike_texture);

	// для картинки вертикального шипа
	sf::Image ver_spike_image;
	ver_spike_image.loadFromFile("images/ver_spike.png");
	ver_spike_image.createMaskFromColor(sf::Color::White);
	sf::Texture ver_spike_texture;
	ver_spike_texture.loadFromImage(ver_spike_image);
	sf::Sprite ver_spike_sprite(ver_spike_texture);

	// для отображения текущего счета
	sf::Font score_font;
	score_font.loadFromFile("fonts/a_Alterna.ttf");
	sf::Text score_text_0(sf::String("0"), score_font, 36);
	score_text_0.setPosition(100, 100);
	score_text_0.setColor(sf::Color::Black);
	sf::Text score_text_1(sf::String("0"), score_font, 36);
	score_text_1.setPosition(360, 100);
	score_text_1.setColor(sf::Color::Black);

	// само поле
	dtts::Field field(width, height, hor_spike_base, hor_spike_height,ver_spike_base,ver_spike_height, complexity, time_step);
	// добавление объектов
	auto birds = make_objects(field);
	dtts::BirdState bird_state_0 = birds.first;
	dtts::BirdState bird_state_1 = birds.second;

	bool is_game_0 = false;		// ? игра идет для этой 0 птицы
	bool is_game_1 = false;		// ? игра идет для этой 1 птицы

	while (window.isOpen()) {
		// обработка закрытия окна
		sf::Event Event;
		while (window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed) {
				window.close();
			}
			if (Event.type == sf::Event::MouseButtonPressed) {
				if (Event.mouseButton.button == sf::Mouse::Left) {
					// действие для 0 птицы
					// если игра не идет
					if (!is_game_0) {
						// игра начинается при нажатии на клавишу
						is_game_0 = true;
					}
					// постольку поскольку нажата мышка, взмах имеет место быть
					field.bird_swing(0);
				}
				else if (Event.mouseButton.button == sf::Mouse::Right) {
					// действие для 1 птицы
					// если игра не идет
					if (!is_game_1) {
						// игра начинается при нажатии на клавишу
						is_game_1 = true;
					}
					// постольку поскольку нажата мышка, взмах имеет место быть
					field.bird_swing(1);
				}
				else if (Event.mouseButton.button == sf::Mouse::Middle) {
					// игра начинается заново
					field.erase_bird(1);
					field.erase_bird(0);
					birds = make_objects(field);
					bird_state_0 = birds.first;
					bird_state_1 = birds.second;
					is_game_0 = false;
					is_game_1 = false;
				}
			}
		}

		// если игра идет
		// птица летит, и получаем ее новое состояние
		if (is_game_0) 
			bird_state_0 = field.bird_fly(0);
		if (is_game_1)
			bird_state_1 = field.bird_fly(1);
			

		// получаем все шипы на поле
		auto spikes = field.get_hor_spikes();

		// получаем все кружочки на поле
		auto circles = field.get_circles();
		
		// получаем счет
		score_text_0.setString(sf::String(std::to_string(bird_state_0.score)));
		score_text_1.setString(sf::String(std::to_string(bird_state_1.score)));

		// очистка старого изображения
		window.clear(sf::Color::White);


		// для отображение кружочка
		sf::CircleShape circle;
		circle.setFillColor(sf::Color::White);
		circle.setOutlineThickness(1.5);
		circle.setOutlineColor(sf::Color::Green);
		// отображение кружочков
		for (const auto &one_bird_circles : circles) {
			for (const auto &one_circle : one_bird_circles) {
				circle.setPosition(one_circle.x - one_circle.r, height - (one_circle.y + one_circle.r));
				circle.setRadius(one_circle.r);
				window.draw(circle);
			}
			circle.setOutlineColor(sf::Color::Red);
		}

		// прорисовка горизонтальных шипов
		for (const auto &one_spike : spikes) {
			hor_spike_sprite.setPosition(one_spike.x - (one_spike.x == 0 ? 0 : hor_spike_height), height - (one_spike.y + hor_spike_base / 2));
			hor_spike_sprite.setTextureRect(sf::IntRect(one_spike.x == 0 ? 0 : hor_spike_height, 0, hor_spike_height, hor_spike_base));
			window.draw(hor_spike_sprite);
		}

		// прорисовка вертикальных шипов
		for (unsigned n = 0; n != std::round(width / ver_spike_base); ++n) {
			ver_spike_sprite.setPosition(n * ver_spike_base, 0);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 0, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
			ver_spike_sprite.setPosition(n * ver_spike_base, height - ver_spike_height);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 1, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
		}

		// прорисовка птиц
		draw_bird(bird_state_0, (bird_state_0.alive ? bird_sprite_0 : dead_bird_sprite), window);
		draw_bird(bird_state_1, (bird_state_1.alive ? bird_sprite_1 : dead_bird_sprite), window);

		// отображение счетов
		window.draw(score_text_0);
		window.draw(score_text_1);

		// отображение нового кадра
		window.display();

		// задержка времени
		Sleep(time_step*1000.0);
	}

	return 0;
}
