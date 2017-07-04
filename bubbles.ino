#include <Servo.h>

Servo bubbleServo;

int fanControl = 3;
int button = 4;
int adjustmentPin = A0;
int batteryPin = A1;
int indicator = LED_BUILTIN; //Pin 13 - connect the indicator LED to this pin!

//Variables
int adjustmentValue = 0;
int servoDownPosition = 0;
int servoUpPosition = 90;
int currentPosition = 0;
int servoDelay = 23; //Approx 2 seconds to bring up or down
int upDelay = 10000; 
boolean isRunning = false;

int batteryLevel = 0;
int batteryBlinkDelay = 1000;
boolean batteryLow = false;
int lowBatteryLevel = 469; //3.3V with 6 times dilution and 1V external reference

void setup() {

	bubbleServo.attach(9);

	pinMode(fanControl, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	pinMode(indicator, OUTPUT);

	analogReference(EXTERNAL);

}


void loop() {

	if(!batteryLow){

		if( digitalRead(button) == LOW ){

			digitalWrite(indicator, HIGH);
			analogWrite(fanControl, 255); //Turn fan full on
			moveUp();
			delay(upDelay);
			analogWrite(fanControl, 127);
			moveDown();
			digitalWrite(indicator, LOW);

		}

		calibrateServo();

	} else{

		digitalWrite(indicator, HIGH);
		delay(batteryBlinkDelay);
		digitalWrite(indicator, LOW);
		delay(batteryBlinkDelay);
		
	}

	checkBattery();

}

int newDownPosition = 0;
void calibrateServo(){

	adjustmentValue = analogRead(adjustmentPin);

	newDownPosition = adjustmentValue/94; //Lazy map of 1023 to a 10-grade scale

	//Check if the calibration value has been updated
	if(newDownPosition != servoDownPosition){

		servoDownPosition = newDownPosition;

		if(!isRunning){
			moveDown();
		}

	}

}

/*Helper method to move servo to down position*/
void moveDown(){

	if(currentPosition != servoDownPosition){

		if(currentPosition < servoDownPosition){
			//If calibration is under way, and we want to raise the spade

			for(int pos = currentPosition; pos <= servoDownPosition; pos++){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		} else if (currentPosition > servoDownPosition){

			//Normally, we simply bring the spade down
			
			for(int pos = currentPosition; pos >= servoDownPosition; pos--){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		}

		currentPosition = servoDownPosition;

	}

}

/*Helper method to move servo to up position*/
void moveUp(){

	if(currentPosition != servoUpPosition){

		if(currentPosition > servoUpPosition){
			//Should strictly never happen. But included for posterity. 

			for(int pos = currentPosition; pos >= servoUpPosition; pos--){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		} else if (currentPosition < servoUpPosition){

			//Normally, we simply bring the spade up
			
			for(int pos = currentPosition; pos <= servoUpPosition; pos++){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		}

		currentPosition = servoUpPosition;

	}

}

// int newBatteryLevel = 0;
void checkBattery(){

	batteryLevel = analogRead(batteryPin);
	//Check if battery level is in the threshold
	batteryLow = batteryLevel < lowBatteryLevel && batteryLevel > 100;

	if(batteryLow){
		analogWrite(fanControl, 0);
		moveDown();
	}

}