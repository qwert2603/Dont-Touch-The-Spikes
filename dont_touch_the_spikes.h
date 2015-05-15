#ifndef DONT_TOUCH_THE_SPIKES
#define DONT_TOUCH_THE_SPIKES

#include <vector>
#include <utility>

/*****************************************************************************/

namespace dont_touch_the_spikes {

	// потом надо будет добавить значения по умолчанию во все консрукторы

	class Bird;			// одна птица
	class Field;		// игровое поле
	class Spike;		// один шип. содержит местолопожение одного шипа
	class SpikeType;	// тип шипов. содержит высоту и диаметр основания шипов
	struct BirdState;	// состояние птицы

	/*****************************************************************************/

	struct BirdState {
		BirdState(double _x, double _y, bool _right_direction, bool _alive, unsigned _score) :
		x(_x), y(_y), right_direction(_right_direction), alive(_alive), score(_score) {}
		double x, y;
		bool right_direction;	// ? летит вправо
		bool alive;				// ? живая ли
		unsigned score;
	};

	class Bird {
	public:
		Bird(Field *_field, double _x, double _y, double _size, double _hor_speed,
			double _ver_speed, double _swing_speed, double _speed_up);
		// птица двигается; (падает или взлетает в зависимости от параметра)
		// вернет свое новое состояние
		BirdState step(bool _is_swing);
		// проверка столкновений с шипами;
		// если ударилась о шипы, вернет true
		bool check_spikes();
		// проверка столкновения с боковыми стенками;
		// при ударе об боковую стену, горизонтальная скорость меняется на противоположную
		// если столкновение было, вернет true
		bool check_walls();
		void motion();				// движение по горизонтали и вертикали с текущей скоростью
	private:
		Field *field;				// указатель на поле, в котором летает птичка
		double x, y;				// текущие координаты
		double size;				// "диаметр" птички
		double hor_speed;			// текущая скорость по горизонтали
		double ver_speed;			// текущая скорость по вертикальная
		double swing_speed;			// начальная вертикальная скорость после взмаха
		double speed_up;			// ускорение, с которым птица падает
		unsigned score;				// кол-во набранных очков
		bool alive;					// ? живая ли
	};

	class SpikeType {
		friend class Field;
		friend class Bird;
	public:
		SpikeType(double _height, double _base) : height(_height), base(_base) {}
	private:
		double height;		// высота шипов
		double base;		// диаметр основания шипов
	};

	class Field {
		friend class Bird;
	public:
		Field(double _width, double _height, double _spike_height, double _spike_base, double _time_step) :
			width(_width), height(_height), spike_type(_spike_height, _spike_base), time_step(_time_step) {}
		// добавить новую птицу, вернет номер новой птицы в std::vector<Bird> birds
		template<typename... Args> std::vector<Bird>::size_type add_bird(Args&&... _args) {
			birds.emplace_back(this, std::forward<Args>(_args)...);
		}
		// птица под этим номером делает шаг (падает); вернет новое состояние птицы
		BirdState bird_fall(std::vector<Bird>::size_type _n) {
			return birds[_n].step(true);
		}
		// птица под этим номером делает шаг (взлетает); вернет новое состояние птицы
		BirdState bird_swing(std::vector<Bird>::size_type _n) {
			return birds[_n].step(false);
		}
		// добавить новый шип
		void add_spike(double _x, double _y) {
			spikes.emplace_back(_x, _y);
		}
		// рандомно добавить рандомное кол-во шипов
		void add_random_spikes();
		// удалить все шипы с поля
		void clear_spikes() { spikes.clear(); }
	private:
		std::vector<Bird> birds;	// птицы на этом поле
		std::vector<Spike> spikes;	// шипы на этом поле
		SpikeType spike_type;		// тип шипов на этом поле
		double width;				// ширина
		double height;				// высота
		double time_step;			// шаг по времени (использется во время хода птиц)
	};

	class Spike {
		friend class Bird;
	public:
		Spike(double _x, double _y) : x(_x), y(_y) {}
	private:
		double x, y;	// координаты основания шипа
	};

	/*****************************************************************************/

	Bird::Bird(Field *_field, double _x, double _y, double _size, double _hor_speed,
		double _ver_speed, double _swing_speed, double _speed_up) :
		field(_field), x(_x), y(_y), size(_size), hor_speed(_hor_speed), ver_speed(_ver_speed),
		swing_speed(_swing_speed), speed_up(_speed_up), score(0), alive(true) {}

	BirdState Bird::step(bool _is_swing) {
		// только живая птица может порхать и падать; мертвая просто улетает куда-то
		if (alive) {
			if (_is_swing) {
				ver_speed = swing_speed;
			}
			else {
				ver_speed -= speed_up * field->time_step;
			}
		}
		// не важно, живая птица или нет, она движется с текущей скоростью
		motion();
		// только живая птица может набрать очки или умереть от шипов
		if (alive) {
			// проверим, не было ли столкновений с шипами
			alive = check_spikes();
			if (alive && check_walls()) {
				// если все еще живая и если ударилась о стену, то счет увеличивается
				++score;
			}
			if (!alive) {
				// мертвая птица улетает в противоположном направлении
				ver_speed *= -1.26;
				hor_speed *= -1.26;
			}
		}
		return BirdState(x, y, hor_speed > 0, alive, score);
	}

	void Bird::motion() {
		double &time_step = field->time_step;
		x += hor_speed * time_step;
		y += ver_speed * time_step;
	}

	bool Bird::check_walls() {
		if (x < size/2) {
			x += size/2 - x;
			hor_speed *= -1.0;
			return true;
		}
		if (x > field->width - size/2) {
			x -= (size/2) - (field->width - x);
			hor_speed *= -1.0;
			return true;
		}
		return false;
	}

	bool Bird::check_spikes() {
		double &spike_height = field->spike_type.height;
		double &spike_base = field->spike_type.base;
		if ((x - size / 2.0) > spike_height && (x + size / 2.0) < field->width - spike_height) {
			// птица находится посередине поля, шипы далеко
			return true;
		}
		if ((y - size / 2.0) < spike_height || (y + size / 2.0) > field->height - spike_height) {
			// птица ударилась об пол или потолок
			return false;
		}
		for (const Spike &one_spike : field->spikes) {
			
		}
		return true;
	}

}	// namespace dont_touch_the_spikes end

#endif
