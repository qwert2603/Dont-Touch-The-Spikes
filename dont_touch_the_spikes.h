#ifndef DONT_TOUCH_THE_SPIKES
#define DONT_TOUCH_THE_SPIKES

#include <vector>
#include <utility>
#include <random>
#include <ctime>
#include <algorithm>

/*****************************************************************************/

namespace dont_touch_the_spikes {

	// потом надо будет добавить значения по умолчанию во все консрукторы

	class Bird;				// одна птица
	class Field;			// игровое поле
	class HorSpike;			// один горизонтальный шип. содержит местолопожение одного шипа
	struct BirdState;		// состояние птицы
	struct HorSpikeState;	// состояние горизонтального шипа

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

	struct HorSpikeState {
		HorSpikeState(double _x, double _y) :
			x(_x), y(_y) {}
		double x, y;	// координаты центра основания
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

	class Field {
		friend class Bird;
	public:
		Field(double _width, double _height, double _hor_spike_base,
			double _hor_spike_height, double _ver_spike_base,
			double _ver_spike_height, unsigned _complexity, double _time_step);
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
			hor_spikes.emplace_back(_x, _y);
		}
		// рандомно добавить рандомное горизонтальных кол-во шипов
		// стена на которую добавлять задается параметром
		void add_random_hor_spikes(bool _to_right);
		// удалить все горизонтальные шипы с поля
		void clear_hor_spikes() { hor_spikes.clear(); }
		void erase_bird(std::vector<Bird>::size_type _n) {
			std::iter_swap(birds.begin() + _n, --birds.end());
			birds.erase(--birds.end());
		}
		// вектор всех птиц на поле
		std::vector<BirdState> get_birds() const;
		// вектор всех горизонтальных шипов на поле
		std::vector<HorSpikeState> get_hor_spikes() const;
	private:
		std::vector<Bird> birds;		// птицы на этом поле
		// горизонтальные шипы на этом поле
		// вертикальные шипы ровным слоем заполняют пол и потолок поля
		// их незачем хранить
		std::vector<HorSpike> hor_spikes;
		// параметры вертикальных шипов на этом поле (пол и потолок)
		double ver_spike_base;			// диаметр основания
		double ver_spike_height;		// высота
		// параметры горизонтальных шипов на этом поле (боковые стенки)
		double hor_spike_base;			// диаметр основания
		double hor_spike_height;		// высота
		double width;					// ширина поля
		double height;					// высота поля
		// шаг по времени (использется во время хода птиц)
		double time_step;
		// сложность  (0 -- 10) (влияет на среднее кол-во горизонтальных шипов)
		unsigned complexity;
		// процессор случайных чисел для этого поля
		std::default_random_engine rand_eng;
	};

	class HorSpike {
		friend class Bird;
		friend class Field;
	public:
		HorSpike(double _x, double _y) : x(_x), y(_y) {}
	private:
		double x, y;	// координаты основания шипа
	};

	/*****************************************************************************/

	inline Bird::Bird(Field *_field, double _x, double _y, double _radius,
		double _hor_speed, double _ver_speed, double _swing_speed, double _speed_up) :
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
				ver_speed *= -2.26;
				hor_speed *= -2.26;
				speed_up *= 0.54;
			}
		}
		return get_state();
	}

	inline void Bird::motion() {
		double &time_step = field->time_step;
		x += hor_speed * time_step;
		y += ver_speed * time_step;
	}

	inline bool Bird::check_walls() {
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
		if ((y - radius) < field->ver_spike_height || (y + radius) > (field->height - field->ver_spike_height)) {
			// птица ударилась об пол или потолок
			return true;
		}
		if ((x - radius) > field->hor_spike_height && (x + radius) < (field->width - field->hor_spike_height)) {
			// птица находится посередине поля, шипы далеко
			return false;
		}
		for (const HorSpike &one_spike : field->hor_spikes) {
			if ((y + radius < one_spike.y - field->hor_spike_base / 2) || (y - radius > one_spike.y + field->hor_spike_base / 2)) {
				// птица находится выше или ниже шипа => шип не опасен
				continue;
			}
			// вершина у основания шипа
			double x1 = one_spike.x;
			double y1 = one_spike.y + (field->hor_spike_base / 2) * (y < one_spike.y ? -1.0 : 1.0);
			// вершина шипа, не лежащая на основании
			double x2 = one_spike.x + (field->hor_spike_height * (one_spike.x == 0 ? 1.0 : -1.0));
			double y2 = one_spike.y;
			// коэф-ты прямой, проходящей через эти 2 точки
			double a = y1 - y2;
			double b = x2 - x1;
			double c = x1*y2 - x2*y1;
			// расстояние от "центра" птицы до этой прямой
			double r = abs(a*x + b*y + c) / (sqrt(pow(a, 2.0) + pow(b, 2.0)));
			// если это расстояние меньше "радиуса" птицы,
			// значит, она коснулась шипа
			if (r < radius) {
				return true;
			}
		}
		// если птица не столкнулась ни с одним шипом, она живая
		return false;
	}

	inline BirdState Bird::get_state() const {
		return BirdState(x, y, hor_speed > 0, alive, score);
	}

	inline Field::Field(double _width, double _height,
		double _hor_spike_base, double _hor_spike_height,
		double _ver_spike_base, double _ver_spike_height,
		unsigned _complexity, double _time_step) :
			width(_width), height(_height),
			hor_spike_base(_hor_spike_base), hor_spike_height(_hor_spike_height),
			ver_spike_base(_ver_spike_base), ver_spike_height(_ver_spike_height),
			complexity(_complexity), time_step(_time_step),
			rand_eng(static_cast<unsigned>(std::time(nullptr))) {}

	void Field::add_random_hor_spikes(bool _to_right) {
		// максимальное возможное число шипов;
		// горизонтальные шипы не должны пересекаться с вертикальными
		double max_spikes = (height - (2 * ver_spike_height)) / (hor_spike_base * 1.67);
		// нормальное распределение случайных чисел для кол-ва шипов
		std::normal_distribution<> rand_skikes_count(max_spikes * complexity / 10, 1.2);
		// число шипов, которые появятся
		unsigned spikes_count = max_spikes + 2;
		while (spikes_count > max_spikes){
			spikes_count = static_cast<unsigned>(abs(rand_skikes_count(rand_eng)));
		}
		// равномерное распределение случайных чисел для местоположения шипов
		std::uniform_real_distribution<> rand_spikes_position(ver_spike_height + hor_spike_base / 2, height - (ver_spike_height + hor_spike_base / 2));
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
				for (const HorSpike &one_spike : hor_spikes) {
					if (x_new == one_spike.x && abs(y_new - one_spike.y) < (hor_spike_base)) {
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

	inline std::vector<BirdState> Field::get_birds() const {
		std::vector<BirdState> result;
		for (const Bird &one_bird : birds) {
			result.emplace_back(one_bird.get_state());
		}
		return result;
	}

	inline std::vector<HorSpikeState> Field::get_hor_spikes() const {
		std::vector<HorSpikeState> result;
		for (const HorSpike &one_spike : hor_spikes) {
			result.emplace_back(one_spike.x, one_spike.y);
		}
		return result;
	}

}	// namespace dont_touch_the_spikes end

#endif