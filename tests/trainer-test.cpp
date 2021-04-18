#include "ai/ai-trainer.h"

int main() {
	AIPopulation pop(10);
	AITrainer trainer(&pop);
	trainer.runTraining(0, 8);
	pop.printInfo();
	pop.save("trainertest.cai");
	AIPopulation load("trainertest.cai");
	std::cout << "Checking: ";
	if (pop == load) {
		std::cout << "PASS";
	}
	else {
		std::cout << "FAIL";
	}
	std::cout << '\n';
}
