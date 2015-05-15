#include "dont_touch_the_spikes.h"
#include <iostream>
#include <vector>
using namespace std;

int main() {
	using dont_touch_the_spikes::Field;
	using dont_touch_the_spikes::BirdState;
	Field f1(80.0, 50.0, 1.0, 1.0, 4, 1.0);
	f1.add_bird(40.0, 25.0, 1.0, 2.5, 2.0, 5.0, 0.6);
	f1.add_random_spikes(true);
	BirdState bs1 = f1.bird_fly(0);
	bs1 = f1.bird_fly(0);
	bs1 = f1.bird_fly(0);
	bs1 = f1.bird_fly(0);
	bs1 = f1.bird_fly(0);
	f1.bird_swing(0);
	bs1 = f1.bird_fly(0);
	for (;;) {
		bs1 = f1.bird_fly(0);
	}

	
	return 0;
}
