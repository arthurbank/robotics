// Define the pins for the ultrasonic sensor and the LED
const int trigPin = 9;   // Trigger pin of the ultrasonic sensor
const int echoPin = 10;  // Echo pin of the ultrasonic sensor
const int ledPin = 13;   // LED connected to digital pin 13

// Define constants for calculating distance
// Speed of sound in air at 20°C is approximately 343 meters per second.
// This translates to 0.0343 cm per microsecond, or 0.0135 inches per microsecond.
const float SPEED_OF_SOUND_IN_INCHES_PER_MICROSECOND = 0.0135;

// Define the distance threshold in inches
const float DISTANCE_THRESHOLD_INCHES = 6.0;

void setup() {
  // Initialize serial communication for debugging (optional, but highly recommended)
  Serial.begin(9600);

  // Set the trigger pin as an OUTPUT
  pinMode(trigPin, OUTPUT);
  // Set the echo pin as an INPUT
  pinMode(echoPin, INPUT);
  // Set the LED pin as an OUTPUT
  pinMode(ledPin, OUTPUT);

  // Ensure the trigPin is low initially
  digitalWrite(trigPin, LOW);
}

void loop() {
  // --- Step 1: Clear the trigPin by setting it LOW for a short period ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // Wait for 2 microseconds

  // --- Step 2: Set the trigPin HIGH for 10 microseconds to send a pulse ---
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Keep it HIGH for 10 microseconds
  digitalWrite(trigPin, LOW); // Set it LOW again

  // --- Step 3: Measure the duration of the pulse on the echoPin ---
  // pulseIn() reads a pulse (HIGH or LOW) on a pin. It waits for the pin to go to the specified state,
  // starts timing, then waits for the pin to go to the other state, and stops timing.
  // It returns the length of the pulse in microseconds.
  long duration = pulseIn(echoPin, HIGH);

  // --- Step 4: Calculate the distance based on the duration ---
  // The duration is the time for the sound to travel to the object AND back.
  // So, we divide the duration by 2 to get the one-way travel time.
  // Then, multiply by the speed of sound to get the distance.
  float distanceInches = (duration / 2.0) * SPEED_OF_SOUND_IN_INCHES_PER_MICROSECOND;

  // --- Step 5: Print the distance to the Serial Monitor (for debugging) ---
  Serial.print("Distance: ");
  Serial.print(distanceInches);
  Serial.println(" inches");

  // --- Step 6: Control the LED based on the distance threshold ---
  if (distanceInches < DISTANCE_THRESHOLD_INCHES) {
    digitalWrite(ledPin, HIGH); // Turn LED ON if object is closer than 6 inches
    Serial.println("Object detected! LED ON.");
  } else {
    digitalWrite(ledPin, LOW);  // Turn LED OFF if object is farther than 6 inches
    Serial.println("No object detected. LED OFF.");
  }

  // Add a small delay between readings to stabilize the sensor and avoid erratic behavior
  delay(100); // Wait for 100 milliseconds before the next reading
}
