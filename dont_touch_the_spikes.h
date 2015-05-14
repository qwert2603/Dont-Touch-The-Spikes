#ifndef DONT_TOUCH_THE_SPIKES
#define DONT_TOUCH_THE_SPIKES

#include <vector>
#include <utility>

namespace dont_touch_the_spikes {

	// потом надо будет добавить значения по умолчанию во все консрукторы

	class Bird;		// одна птица
	class Field;	// игровое поле
	class Spike;	// один шип

	class Bird {
		friend class Field;
	private:
		Bird(Field *_field, double _size, double _hor_speed, double _ver_speed) :
			field(_field), size(_size), hor_speed(_hor_speed), ver_speed(_ver_speed) {}

		void swing();				// взмах
		bool check_spikes() const;	// проверка столкновений с шипами, если все хорошо, вернет true
	private:
		Field *field;				// указатель на поле, в котором летает птичка
		double size;				// "диаметр" птички
		double x, y;				// текущие координаты
		double hor_speed;			// текущая скорость по горизонтали
		double ver_speed;			// текущая скорость по вертикальная
		double swing_speed;			// начальная вертикальная скорость после взмаха
		unsigned score;				// кол-во набранных очков
	};

	class Field {
		friend class Bird;
	public:
		Field(double _width, double _height) : width(_width), height(_height) {}
		// добавить новую птицу
		template<typename... Args> std::vector<Bird>::size_type add_bird(Args&&... _args) {
			birds.emplace_back(std::forward<Args>(_args)...);
		}
		
	private:
		// добавить новый шип
		template<typename... Args> std::vector<Spike>::size_type add_spike(Args&&... _args) {
			spikes.emplace_back(std::forward<Args>(_args)...);
		}
		// рандомно добавить рандомное кол-во шипов
		void add_random_spikes();
		// удалить все шипы с поля
		void clear_spikes() { spikes.clear(); }
	private:
		std::vector<Bird> birds;	// птицы на этом поле
		std::vector<Spike> spikes;	// шипы на этом поле
		double width;				// ширина
		double height;				// высота
	};

	class Spike {
		friend class Bird;
	private:
		Spike(double _height, double _base) :height(_height), base(_base){}
	private:
		double height;				// высота шипа
		double base;				// диаметр основания шипа 
	};

}	// namespace dont_touch_the_spikes end

#endif
