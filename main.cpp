#include <iostream>
#include <fstream>
#include <Windows.h>
#include <SFML\Graphics.hpp>
#include "dont_touch_the_spikes.h"
namespace dtts = dont_touch_the_spikes;

// размеры окна
const unsigned width = 520;
const unsigned height = 640;
// радиус птицы
const double bird_radius = 32;
// размеры шипов на боковых стенках
const double hor_spike_base = 32;
const double hor_spike_height = 32;
// размеры шипов на полу и потолке
const double ver_spike_base = 16;
const double ver_spike_height = 16;
// шаг по времени
const double time_step = 0.015;

// рисует заданную птицу в нужном месте в переданном окне
void draw_bird(const dtts::BirdState &_bird_state, sf::Sprite &_sprite, sf::RenderWindow &_window) {
	_sprite.setPosition(_bird_state.x - bird_radius, height - (_bird_state.y + bird_radius));
	_sprite.setTextureRect(sf::IntRect(_bird_state.right_direction ? 0 : 2 * bird_radius, 0, bird_radius * 2, bird_radius * 2));
	_window.draw(_sprite);
}

// добавить птицу и шипы на поле
dtts::BirdState make_bird_n_spikes(dtts::Field &_field) {
	// добавление птицы
	_field.add_bird(width / 2, height / 2, bird_radius, 280, 0, 300, 470);
	// удалить горизонтальные шипы, которые уже есть на поле
	_field.clear_hor_spikes();
	// добавить шипы справа
	_field.add_random_hor_spikes(true);
	// вернуть состояние птицы
	return _field.get_birds()[0];
}

int main() {	
	// само окно
	sf::RenderWindow window(sf::VideoMode(width, height), "_dtts");

	// для картинки птицы
	sf::Image bird_image;
	bird_image.loadFromFile("images/bird.png");
	bird_image.createMaskFromColor(sf::Color::White);
	sf::Texture bird_texture;
	bird_texture.loadFromImage(bird_image);
	sf::Sprite bird_sprite(bird_texture);

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
	sf::Text score_text(sf::String("0"), score_font, 36);
	score_text.setPosition(100, 100);
	score_text.setColor(sf::Color::Black);

	// поле
	dtts::Field field(width, height, hor_spike_base, hor_spike_height,ver_spike_base,ver_spike_height, 4, time_step);
	// начальное состояние птицы
	dtts::BirdState bird_state = make_bird_n_spikes(field);

	bool bird_to_right = true;	// ? птица летит вправо
	bool is_game = false;		// ? игра идет
	// время, прошедшее после смерти птицы
	double after_death_counter = 0;

	while (window.isOpen()) {
		// обработка закрытия окна
		sf::Event Event;
		while (window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed)
				window.close();
		}

		// если игра идет
		if (is_game) {
			// взмах
			if (bird_state.alive && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				field.bird_swing(0);
			}
			// птица летит
			bird_state = field.bird_fly(0);
			//	если птица мертва, считаем время
			if (!bird_state.alive) {
				after_death_counter += time_step;
			}
			// если птица мертва достаточно долго, игра прекращается,
			// старая птица удаляется, и создается новая птица, аналогичная старой,
			// также создаются новые шипы
			if (after_death_counter > 1.0) {
				is_game = false;
				after_death_counter = 0;
				field.erase_bird(0);
				bird_state = make_bird_n_spikes(field);
			}
			// если живая птица ударилась о стену и поменяла направление
			if (bird_state.alive && bird_state.right_direction != bird_to_right) {
				// запоминаем новое направление
				bird_to_right = bird_state.right_direction;
				// удаляем все шипы
				field.clear_hor_spikes();
				// добавляем шипы на противоположную стену
				field.add_random_hor_spikes(bird_to_right);
			}
		}
		else {
			// игра начинается при нажатии на клавишу
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				is_game = true;
			}
		}

		// получаем все шипы на поле
		auto spikes = field.get_hor_spikes();

		// очистка старого изображения
		window.clear(sf::Color::White);

		// прорисовка горизонтальных шипов
		for (const auto &one_spike : spikes) {
			hor_spike_sprite.setPosition(one_spike.x - (one_spike.x == 0 ? 0 : hor_spike_height), height - (one_spike.y + hor_spike_base / 2));
			hor_spike_sprite.setTextureRect(sf::IntRect(one_spike.x == 0 ? 0 : hor_spike_height, 0, hor_spike_height, hor_spike_base));
			window.draw(hor_spike_sprite);
		}

		// прорисовка вертикальных шипов
		for (unsigned n = 0; n != round(width / ver_spike_base); ++n) {
			ver_spike_sprite.setPosition(n * ver_spike_base, 0);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 0, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
			ver_spike_sprite.setPosition(n * ver_spike_base, height - ver_spike_height);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 1, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
		}

		// прорисовка птицы
		draw_bird(bird_state, (bird_state.alive ? bird_sprite : dead_bird_sprite), window);

		// счет меняется только во время игры
		if (is_game) {
			score_text.setString(sf::String(std::to_string(bird_state.score)));
		}
		// отображение счета
		window.draw(score_text);

		// отображение нового кадра
		window.display();

		// задержка времени
		Sleep(time_step*1000.0);
	}

	return 0;
}
