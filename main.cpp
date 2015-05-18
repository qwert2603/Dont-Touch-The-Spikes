#include <iostream>
#include <fstream>
#include <Windows.h>
#include <SFML\Graphics.hpp>
#include "dont_touch_the_spikes.h"
namespace dtts = dont_touch_the_spikes;

int main() {
	// размеры окна
	const unsigned width = 480;
	const unsigned height = 640;
	// радиус птицы
	const double bird_radius = 24;
	// размеры шипов на боковых стенках
	const double hor_spike_base = 32;
	const double hor_spike_height = 32;
	// размеры шипов на полу и потолке
	const double ver_spike_base = 16;
	const double ver_spike_height = 16;
	// шаг по времени
	const double time_step = 0.025;
	
	// само окно
	sf::RenderWindow window(sf::VideoMode(width, height), "_dtts");
	// для картинки птицы
	sf::Texture bird_texture;
	bird_texture.loadFromFile("images/bird.png");
	sf::Sprite bird_sprite(bird_texture);
	// для картинки горизонтального шипа
	sf::Texture hor_spike_texture;
	hor_spike_texture.loadFromFile("images/hor_spike.png");
	sf::Sprite hor_spike_sprite(hor_spike_texture);
	// для картинки вертикального шипа
	sf::Texture ver_spike_texture;
	ver_spike_texture.loadFromFile("images/ver_spike.png");
	sf::Sprite ver_spike_sprite(ver_spike_texture);

	// поле
	dtts::Field field(width, height, hor_spike_base, hor_spike_height,ver_spike_base,ver_spike_height, 2, time_step);
	// добавление птицы
	field.add_bird(width / 2, height / 2, bird_radius, 30, 15, 31, 18);
	// добавить шипы справа
	field.add_random_hor_spikes(true);

	bool bird_to_right = true;	// ? птица летит вправо

	while (window.isOpen()) {
		// обработка закрытия окна
		sf::Event Event;
		while (window.pollEvent(Event)) {
			if (Event.type == sf::Event::Closed)
				window.close();
		}

		// взмах
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			field.bird_swing(0);
		}
		// птица летит
		dtts::BirdState bird_state = field.bird_fly(0);		
		// если живая птица ударилась о стену и поменяла направление
		if (bird_state.alive && bird_state.right_direction != bird_to_right) {
			// запоминаем новое направление
			bird_to_right = bird_state.right_direction;
			// удаляем все шипы
			field.clear_hor_spikes();
			// добавляем шипы на противоположную стену
			field.add_random_hor_spikes(bird_to_right);
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
		for (unsigned n = 0; n != width / ver_spike_base; ++n) {
			ver_spike_sprite.setPosition(n * ver_spike_base, 0);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 0, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
			ver_spike_sprite.setPosition(n * ver_spike_base, height - ver_spike_height);
			ver_spike_sprite.setTextureRect(sf::IntRect(ver_spike_base * 1, 0, ver_spike_base, ver_spike_height));
			window.draw(ver_spike_sprite);
		}

		// прорисовка птицы
		bird_sprite.setPosition(bird_state.x - bird_radius, height - (bird_state.y + bird_radius));
		bird_sprite.setTextureRect(sf::IntRect(bird_state.right_direction ? 0 : 2 * bird_radius, 0, bird_radius * 2, bird_radius * 2));
		window.draw(bird_sprite);

		// отображение нового кадра
		window.display();

		// задержка времени
		Sleep(time_step);
	}

	return 0;
}
