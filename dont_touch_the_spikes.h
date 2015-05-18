#ifndef DONT_TOUCH_THE_SPIKES
#define DONT_TOUCH_THE_SPIKES

#include <vector>
#include <utility>
#include <random>
#include <ctime>
#include <algorithm>

/*****************************************************************************/

namespace dont_touch_the_spikes {

	// ����� ���� ����� �������� �������� �� ��������� �� ��� �����������

	class Bird;				// ���� �����
	class Field;			// ������� ����
	class HorSpike;			// ���� �������������� ���. �������� �������������� ������ ����
	struct BirdState;		// ��������� �����
	struct HorSpikeState;	// ��������� ��������������� ����

	/*****************************************************************************/

	struct BirdState {
		BirdState(double _x, double _y, bool _right_direction,
			bool _alive, unsigned _score) :
				x(_x), y(_y), right_direction(_right_direction),
				alive(_alive), score(_score) {}
		double x, y;
		bool right_direction;	// ? ����� ������
		bool alive;				// ? ����� ��
		unsigned score;
	};

	struct HorSpikeState {
		HorSpikeState(double _x, double _y) :
			x(_x), y(_y) {}
		double x, y;	// ���������� ������ ���������
	};

	class Bird {
	public:
		Bird(Field *_field, double _x, double _y, double _size, double _hor_speed,
			double _ver_speed, double _swing_speed, double _speed_up);
		// ����� �����; ������ ���� ����� ���������
		BirdState fly();
		// �����, �������� �� ��������� ���������� ������ swing_speed
		void swing() {
			ver_speed = swing_speed;
		}
		// �������� ������������ � ������;
		// ���� ��������� � ����, ������ true
		bool check_spikes();
		// �������� ������������ � �������� ��������;
		// ��� ����� �� ������� �����,
		// �������������� �������� �������� �� ���������������
		// ���� ������������ ����, ������ true
		bool check_walls();
		// �������� �� ����������� � ��������� � ������� ���������
		void motion();
		// ������� ��������� �����
		BirdState get_state() const;
	private:
		Field *field;				// ��������� �� ����, � ������� ������ ������
		double x, y;				// ������� ����������
		double radius;				// "������" ������
		double hor_speed;			// ������� �������� �� �����������
		double ver_speed;			// ������� �������� �� ������������
		double swing_speed;			// ��������� ������������ �������� ����� ������
		double speed_up;			// ���������, � ������� ����� ������
		unsigned score;				// ���-�� ��������� �����
		bool alive;					// ? ����� ��
	};

	class Field {
		friend class Bird;
	public:
		Field(double _width, double _height, double _hor_spike_base,
			double _hor_spike_height, double _ver_spike_base,
			double _ver_spike_height, unsigned _complexity, double _time_step);
		// �������� ����� �����, ������ ����� ����� ����� � std::vector<Bird> birds
		std::vector<Bird>::size_type add_bird(double _x, double _y,
			double _radius, double _hor_speed, double _ver_speed,
			double _swing_speed, double _speed_up) {
			birds.push_back(Bird(this, _x, _y, _radius, _hor_speed,
				_ver_speed, _swing_speed, _speed_up));
			return birds.size() - 1;
		}
		// ����� ��� ���� ������� ������ ��� (������); ������ ����� ��������� �����
		BirdState bird_fly(std::vector<Bird>::size_type _n) {
			return birds[_n].fly();
		}
		// ����� ��� ���� ������� ������ �����
		void bird_swing(std::vector<Bird>::size_type _n) {
			birds[_n].swing();
		}
		// �������� ����� ���
		void add_spike(double _x, double _y) {
			hor_spikes.emplace_back(_x, _y);
		}
		// �������� �������� ��������� �������������� ���-�� �����
		// ����� �� ������� ��������� �������� ����������
		void add_random_hor_spikes(bool _to_right);
		// ������� ��� �������������� ���� � ����
		void clear_hor_spikes() { hor_spikes.clear(); }
		void erase_bird(std::vector<Bird>::size_type _n) {
			std::iter_swap(birds.begin() + _n, --birds.end());
			birds.erase(--birds.end());
		}
		// ������ ���� ���� �� ����
		std::vector<BirdState> get_birds() const;
		// ������ ���� �������������� ����� �� ����
		std::vector<HorSpikeState> get_hor_spikes() const;
	private:
		std::vector<Bird> birds;		// ����� �� ���� ����
		// �������������� ���� �� ���� ����
		// ������������ ���� ������ ����� ��������� ��� � ������� ����
		// �� ������� �������
		std::vector<HorSpike> hor_spikes;
		// ��������� ������������ ����� �� ���� ���� (��� � �������)
		double ver_spike_base;			// ������� ���������
		double ver_spike_height;		// ������
		// ��������� �������������� ����� �� ���� ���� (������� ������)
		double hor_spike_base;			// ������� ���������
		double hor_spike_height;		// ������
		double width;					// ������ ����
		double height;					// ������ ����
		// ��� �� ������� (����������� �� ����� ���� ����)
		double time_step;
		// ���������  (0 -- 10) (������ �� ������� ���-�� �������������� �����)
		unsigned complexity;
		// ��������� ��������� ����� ��� ����� ����
		std::default_random_engine rand_eng;
	};

	class HorSpike {
		friend class Bird;
		friend class Field;
	public:
		HorSpike(double _x, double _y) : x(_x), y(_y) {}
	private:
		double x, y;	// ���������� ��������� ����
	};

	/*****************************************************************************/

	inline Bird::Bird(Field *_field, double _x, double _y, double _radius,
		double _hor_speed, double _ver_speed, double _swing_speed, double _speed_up) :
			field(_field), x(_x), y(_y), radius(_radius), hor_speed(_hor_speed),
			ver_speed(_ver_speed),	swing_speed(_swing_speed),
			speed_up(_speed_up), score(0), alive(true) {}

	BirdState Bird::fly() {
		// �������� ���� �������
		ver_speed -= speed_up * field->time_step;
		// �� �����, ����� ����� ��� ���, ��� �������� � ������� ���������
		motion();
		// ������ ����� ����� ����� ������� ���� ��� ������� �� �����
		if (alive) {
			// ��������, �� ���� �� ������������ � ������
			alive = !check_spikes();
			if (alive && check_walls()) {
				// ���� ��� ��� ����� � ���� ��������� � �����, �� ���� �������������
				++score;
			}
			if (!alive) {
				// ������� ����� ������� � ��������������� �����������
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
			// ����� ��������� �� ��� ��� �������
			return true;
		}
		if ((x - radius) > field->hor_spike_height && (x + radius) < (field->width - field->hor_spike_height)) {
			// ����� ��������� ���������� ����, ���� ������
			return false;
		}
		for (const HorSpike &one_spike : field->hor_spikes) {
			if ((y + radius < one_spike.y - field->hor_spike_base / 2) || (y - radius > one_spike.y + field->hor_spike_base / 2)) {
				// ����� ��������� ���� ��� ���� ���� => ��� �� ������
				continue;
			}
			// ������� � ��������� ����
			double x1 = one_spike.x;
			double y1 = one_spike.y + (field->hor_spike_base / 2) * (y < one_spike.y ? -1.0 : 1.0);
			// ������� ����, �� ������� �� ���������
			double x2 = one_spike.x + (field->hor_spike_height * (one_spike.x == 0 ? 1.0 : -1.0));
			double y2 = one_spike.y;
			// ����-�� ������, ���������� ����� ��� 2 �����
			double a = y1 - y2;
			double b = x2 - x1;
			double c = x1*y2 - x2*y1;
			// ���������� �� "������" ����� �� ���� ������
			double r = abs(a*x + b*y + c) / (sqrt(pow(a, 2.0) + pow(b, 2.0)));
			// ���� ��� ���������� ������ "�������" �����,
			// ������, ��� ��������� ����
			if (r < radius) {
				return true;
			}
		}
		// ���� ����� �� ����������� �� � ����� �����, ��� �����
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
		// ������������ ��������� ����� �����;
		// �������������� ���� �� ������ ������������ � �������������
		double max_spikes = (height - (2 * ver_spike_height)) / (hor_spike_base * 1.67);
		// ���������� ������������� ��������� ����� ��� ���-�� �����
		std::normal_distribution<> rand_skikes_count(max_spikes * complexity / 10, 1.2);
		// ����� �����, ������� ��������
		unsigned spikes_count = max_spikes + 2;
		while (spikes_count > max_spikes){
			spikes_count = static_cast<unsigned>(abs(rand_skikes_count(rand_eng)));
		}
		// ����������� ������������� ��������� ����� ��� �������������� �����
		std::uniform_real_distribution<> rand_spikes_position(ver_spike_height + hor_spike_base / 2, height - (ver_spike_height + hor_spike_base / 2));
		// ��������� ������ ���-�� �����
		while (spikes_count--) {
			// ? ���������� ���� ���������
			bool is_ok = false;
			// ���������� ������ ����
			double x_new;
			double y_new;
			// ���� ��� ������ ���� �� ����� ������� �����
			while (!is_ok) {
				is_ok = true;
				x_new = (_to_right ? width : 0);
				y_new = rand_spikes_position(rand_eng);
				// ��������, �� ������������ �� ����� ��� � ��� ����������
				for (const HorSpike &one_spike : hor_spikes) {
					if (x_new == one_spike.x && abs(y_new - one_spike.y) < (hor_spike_base)) {
						// ����� ��� ������������ � ��� ���������
						is_ok = false;
						break;
					}
				}
			}
			// ��������� ����� ��� �� ����
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