#include <Servo.h>

Servo bubbleServo;

int fanControl = 3;
int button = 4;
int adjustmentPin = A0;
int batteryPin = A2;
int indicator = LED_BUILTIN; //Pin 13 - connect the indicator LED to this pin!

//Variables
int adjustmentValue = 0;
int servoDownPosition = 90;
int servoUpPosition = 0;
int currentPosition = 90;
int servoDelay = 23; //Approx 2 seconds to bring up or down
int upDelay = 10000; 
int downDelay = 1000;
boolean isRunning = false;
unsigned long lastRun = 0;
unsigned long fanTimeout = 30000;

int batteryLevel = 0;
int batteryBlinkDelay = 1000;
boolean batteryLow = false;
int lowBatteryLevel = 900; //6.6V with 6 times dilution and 1.25V external reference

void setup() {

	//Serial.begin(9600);

	bubbleServo.attach(9);
	Serial.println("Moving to 90 at start");
	bubbleServo.write(90);
	moveDown(); //to avoid any jerky movement

	pinMode(fanControl, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	pinMode(indicator, OUTPUT);

	analogReference(EXTERNAL);

}


void loop() {

	if(!batteryLow){

		if( digitalRead(button) == LOW ){

			Serial.println("Button pressed!");

			lastRun = millis();
			digitalWrite(indicator, HIGH);
			moveUp();
			analogWrite(fanControl, 255); //Turn fan full on. Do this after move to save mA.

			Serial.println("Waiting 10 seconds");

			delay(upDelay);
			analogWrite(fanControl, 127);
			moveDown();

			delay(downDelay);
			digitalWrite(indicator, LOW);

		}

		if(millis() - lastRun > fanTimeout){
			analogWrite(fanControl, 0);
		}

		calibrateServo();

	} else{

		Serial.println("Battery level low!");
		digitalWrite(indicator, HIGH);
		delay(batteryBlinkDelay);
		digitalWrite(indicator, LOW);
		delay(batteryBlinkDelay);
		
	}

	checkBattery();

} //Loop


/*Helper method to calibrate the down position of the spade*/
int newDownPosition = 0;
void calibrateServo(){

	adjustmentValue = analogRead(adjustmentPin);

	Serial.println("Calibration raw: "); Serial.print(adjustmentValue);

	newDownPosition = 90 - adjustmentValue/94; //Lazy map of 1023 to a 10-grade scale

	Serial.println("Position raw: "); Serial.print(newDownPosition);

	//Check if the calibration value has been updated
	if(newDownPosition != servoDownPosition){

		Serial.println("Down level updated");

		servoDownPosition = newDownPosition;

		if(!isRunning){
			moveDown();
		}

	}

}

/*Helper method to move servo to down position*/
void moveDown(){

	Serial.println("Moving down");

	if(currentPosition != servoDownPosition){

		if(currentPosition > servoDownPosition){
			//If calibration is under way, and we want to raise the spade

			for(int pos = currentPosition; pos >= servoDownPosition; pos--){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		} else if (currentPosition < servoDownPosition){

			//Normally, we simply bring the spade down
			
			for(int pos = currentPosition; pos <= servoDownPosition; pos++){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		}

		currentPosition = servoDownPosition;

	}

}

/*Helper method to move servo to up position*/
void moveUp(){

	Serial.println("Moving up");

	if(currentPosition != servoUpPosition){

		if(currentPosition < servoUpPosition){
			//Should strictly never happen. But included for posterity. 

			for(int pos = currentPosition; pos <= servoUpPosition; pos++){

				bubbleServo.write(pos);
				delay(servoDelay);

			}

		} else if (currentPosition > servoUpPosition){

			//Normally, we simply bring the spade up
			
			for(int pos = currentPosition; pos >= servoUpPosition; pos--){

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