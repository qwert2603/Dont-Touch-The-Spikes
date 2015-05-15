#ifndef DONT_TOUCH_THE_SPIKES
#define DONT_TOUCH_THE_SPIKES

#include <vector>
#include <utility>
#include <random>
#include <ctime>

/*****************************************************************************/

namespace dont_touch_the_spikes {

	// потом надо будет добавить значения по умолчанию во все консрукторы

	class Bird;			// одна птица
	class Field;		// игровое поле
	class Spike;		// один шип. содержит местолопожение одного шипа
	class SpikeType;	// тип шипов. содержит высоту и диаметр основания шипов
	struct BirdState;	// состояние птицы
	struct SpikeState;	// состояние шипа

	/*****************************************************************************/

	struct BirdState {
		BirdState(double _x, double _y, bool _right_direction,
			bool _alive, unsigned _score) :
				x(_x), y(_y), right_direction(_right_direction),
				alive(_alive), score(_score) {}
		double x, y;
		bool right_direction;	// ? летит вправо
		bool alive;				// ? живая ли
		unsigned score;
	};

	struct SpikeState {
		SpikeState(double _x, double _y, double _base, double _height) :
			x(_x), y(_y), base(_base), height(_height) {}
		double x, y;	// координаты центра основания
		double base;		// диаметр основания шипа
		double height;		// высота шипа
	};

	class Bird {
	public:
		Bird(Field *_field, double _x, double _y, double _size, double _hor_speed,
			double _ver_speed, double _swing_speed, double _speed_up);
		// птица летит; вернет свое новое состояние
		BirdState fly();
		// взмах, скорость по вертикали становится равной swing_speed
		void swing() {
			ver_speed = swing_speed;
		}
		// проверка столкновений с шипами;
		// если ударилась о шипы, вернет true
		bool check_spikes();
		// проверка столкновения с боковыми стенками;
		// при ударе об боковую стену,
		// горизонтальная скорость меняется на противоположную
		// если столкновение было, вернет true
		bool check_walls();
		// движение по горизонтали и вертикали с текущей скоростью
		void motion();
		// текущее состояние птицы
		BirdState get_state() const;
	private:
		Field *field;				// указатель на поле, в котором летает птичка
		double x, y;				// текущие координаты
		double radius;				// "радиус" птички
		double hor_speed;			// текущая скорость по горизонтали
		double ver_speed;			// текущая скорость по вертикальная
		double swing_speed;			// начальная вертикальная скорость после взмаха
		double speed_up;			// ускорение, с которым птица падает
		unsigned score;				// кол-во набранных очков
		bool alive;					// ? живая ли
	};

	class SpikeType {
		friend class Bird;
		friend class Field;
	public:
		SpikeType(double _base, double _height) : base(_base), height(_height) {}
	private:
		double base;		// диаметр основания шипов
		double height;		// высота шипов
	};

	class Field {
		friend class Bird;
	public:
		Field(double _width, double _height, double _spike_base,
			double _spike_height, unsigned _complexity, double _time_step);
		// добавить новую птицу, вернет номер новой птицы в std::vector<Bird> birds
		std::vector<Bird>::size_type add_bird(double _x, double _y,
			double _radius, double _hor_speed, double _ver_speed,
			double _swing_speed, double _speed_up) {
			birds.push_back(Bird(this, _x, _y, _radius, _hor_speed,
				_ver_speed, _swing_speed, _speed_up));
			return birds.size() - 1;
		}
		// птица под этим номером делает шаг (падает); вернет новое состояние птицы
		BirdState bird_fly(std::vector<Bird>::size_type _n) {
			return birds[_n].fly();
		}
		// птица под этим номером делает взмах
		void bird_swing(std::vector<Bird>::size_type _n) {
			birds[_n].swing();
		}
		// добавить новый шип
		void add_spike(double _x, double _y) {
			spikes.emplace_back(_x, _y);
		}
		// рандомно добавить рандомное кол-во шипов
		// стена на которую добавлять задается параметром
		void add_random_spikes(bool _to_right);
		// удалить все шипы с поля
		void clear_spikes() { spikes.clear(); }
		// вектор всех птиц на поле
		std::vector<BirdState> get_birds() const;
		// вектор всех шипов на поле
		std::vector<SpikeState> get_spikes() const;
	private:
		std::vector<Bird> birds;	// птицы на этом поле
		std::vector<Spike> spikes;	// шипы на этом поле
		SpikeType spike_type;		// тип шипов на этом поле
		double width;				// ширина
		double height;				// высота
		double time_step;			// шаг по времени (использется во время хода птиц)
		// сложность  (0 -- 10) (влияет на среднее кол-во шипов)
		unsigned complexity;
		// процессор случайных чисел для этого поля
		std::default_random_engine rand_eng;
	};

	class Spike {
		friend class Bird;
		friend class Field;
	public:
		Spike(double _x, double _y) : x(_x), y(_y) {}
	private:
		double x, y;	// координаты основания шипа
	};

	/*****************************************************************************/

	Bird::Bird(Field *_field, double _x, double _y, double _radius, double _hor_speed,
		double _ver_speed, double _swing_speed, double _speed_up) :
			field(_field), x(_x), y(_y), radius(_radius), hor_speed(_hor_speed),
			ver_speed(_ver_speed),	swing_speed(_swing_speed),
			speed_up(_speed_up), score(0), alive(true) {}

	BirdState Bird::fly() {
		// действие силы тяжести
		ver_speed -= speed_up * field->time_step;
		// не важно, живая птица или нет, она движется с текущей скоростью
		motion();
		// только живая птица может набрать очки или умереть от шипов
		if (alive) {
			// проверим, не было ли столкновений с шипами
			alive = !check_spikes();
			if (alive && check_walls()) {
				// если все еще живая и если ударилась о стену, то счет увеличивается
				++score;
			}
			if (!alive) {
				// мертвая птица улетает в противоположном направлении
				ver_speed *= -1.26;
				hor_speed *= -1.26;
				speed_up *= 0.74;
			}
		}
		return get_state();
	}

	void Bird::motion() {
		double &time_step = field->time_step;
		x += hor_speed * time_step;
		y += ver_speed * time_step;
	}

	bool Bird::check_walls() {
		if (x < radius) {
			x += radius - x;
			hor_speed *= -1.0;
			return true;
		}
		if (x > field->width - radius) {
			x -= radius - (field->width - x);
			hor_speed *= -1.0;
			return true;
		}
		return false;
	}

	bool Bird::check_spikes() {
		double &spike_base = field->spike_type.base;
		double &spike_height = field->spike_type.height;
		if ((y - radius) < spike_height || (y + radius) > field->height - spike_height) {
			// птица ударилась об пол или потолок
			return true;
		}
		if ((x - radius) > spike_height && (x + radius) < field->width - spike_height) {
			// птица находится посередине поля, шипы далеко
			return false;
		}
		for (const Spike &one_spike : field->spikes) {
			if (sqrt(pow(one_spike.x - x, 2.0) + pow(one_spike.y - y, 2.0)) < (radius + spike_height)) {
				// если птица подлетела к центру основания шипа
				// на расстояние меньшее высоты шипа, она умирает
				// если точность будет не достаточной, можно переделать,
				// чтобы проверялись именно границы шипа, а не расстояние до центра его основания
				return true;
			}
		}
		// если птица не столкнулась ни с одним шипом, она живая
		return false;
	}

	BirdState Bird::get_state() const {
		return BirdState(x, y, hor_speed > 0, alive, score);
	}

	Field::Field(double _width, double _height, double _spike_base,
		double _spike_height, unsigned _complexity, double _time_step) :
			width(_width), height(_height), spike_type(_spike_base, _spike_height),
			complexity(_complexity), time_step(_time_step),
			rand_eng(std::time(nullptr)) {}

	void Field::add_random_spikes(bool _to_right) {
		// максимальное возможное число шипов
		double max_spikes = (height / spike_type.base) / 1.26;
		// нормальное распределение случайных чисел для кол-ва шипов
		std::normal_distribution<> rand_skikes_count(max_spikes * complexity / 10, 2.0);
		// число шипов, которые появятся
		unsigned spikes_count = abs(rand_skikes_count(rand_eng));
		// равномерное распределение случайных чисел для местоположения шипов
		std::uniform_real_distribution<> rand_spikes_position(0, height);
		// добавляем нужное кол-во шипов
		while (spikes_count--) {
			// ? добавление шипа выполнено
			bool is_ok = false;
			// координаты нового шипа
			double x_new;
			double y_new;
			// пока для нового шипа не будет найдено место
			while (!is_ok) {
				is_ok = true;
				x_new = (_to_right ? width : 0);
				y_new = rand_spikes_position(rand_eng);
				// проверим, не пересекается ли новый шип с уже имеющимися
				for (const Spike &one_spike : spikes) {
					if (x_new == one_spike.x && abs(y_new - one_spike.y) < (spike_type.base / 2)) {
						// новый шип пересекается с уже имеющимся
						is_ok = false;
						break;
					}
				}
			}
			// добавляем новый шип на поле
			add_spike(x_new, y_new);
		}
	}

	std::vector<BirdState> Field::get_birds() const {
		std::vector<BirdState> result;
		for (const Bird &one_bird : birds) {
			result.push_back(one_bird.get_state());
		}
		return result;
	}

	std::vector<SpikeState> Field::get_spikes() const {
		std::vector<SpikeState> result;
		for (const Spike &one_spike : spikes) {
			result.emplace_back(one_spike.x, one_spike.y, spike_type.base, spike_type.height);
		}
		return result;
	}

}	// namespace dont_touch_the_spikes end

#endif
