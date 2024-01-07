//  flexibly define the "port" that drives the R-2R ladder.
//  The MSB and LSB pins can be re-defined to match which Arduino pins are connected to the resistor ladder.
//  This code assumes that the pins are used in LSB to MSB, in increasing order, with no gaps.  
//  I.e. using 2 for the MSB and 9 for the LSB will not work; neither will using 2, 3, 5, and 6 for the four-bit DAC.
const int LSB_PIN=2;
const int MSB_PIN=9;

//  LEVELS automatically reflects the number of digital steps of the resistor ladder depending on how many output pins are used (number of bits).
const int LEVELS = (1<<(MSB_PIN-LSB_PIN+1));

//  a function that writes an integer value to a range of arduino pins (as defined above), with the arduino pins representing the value in binary.
void write_bits_to_DAC(int level){
  int bit_position = 1;

  // The following loop is used to update pins 2 through 9 without specifying each one.
  for(int j=LSB_PIN;j<=MSB_PIN;j++){

    // " & " is used here to extract the value (0 or 1) of bit j from the value stored in level
    digitalWrite(j,((level & bit_position)>>(j-LSB_PIN))); 

    //  the binary representation of "bit_position" is a single 1 value that gets shifted one bit towards the most signficant bit at each loop iteration.
    //  numerically, this is equivalent to doubling the value of bit_position on each loop iteration.
    bit_position=bit_position<<1; 
 }
}

void setup() {
  // put your setup code here, to run once:
  for(int i=LSB_PIN; i <= MSB_PIN; i++){
    pinMode(i, OUTPUT); // Set pins 2 through 9 as outputs. Don't use pin 0 or 1 as they will mess up the serial monitor
  }
  pinMode(A0, INPUT); // This analog pins will be used to measure the voltage
  Serial.begin(9600); // Start the serial port
}

void loop() {
  // put your main code here, to run repeatedly:

  // Sweep over all the possible DAC outputs.
  for(int i=0; i<LEVELS; i++){

    //  use a function (defined above) to set the Arduino digital outputs according to the binary representation of i.
    //  This could have been written in-line, since the function is only called in one place;
    //  but, it is still useful to wrap the implimentation details into a conceptual block - heiarchical design keeps systems manageable as complexity grows.
    write_bits_to_DAC(i);

    // Read the value on the analog-to-digital converter (ADC) input pin
    delay(100); // Allow time for the circuit to react to the new PWM value
    double value_A0 = analogRead(A0); // Receive the analog value on A0 and A1. These values range from 0 to 1023 and represent a fractional numerator of 5V
    double V0 = ((double) value_A0/1024.0)*5.0; // Cast the value into a fraction and then into voltage

    Serial.print(i);
    Serial.print(" ");
    Serial.print(V0,4); //print V0 with four decimal places
    Serial.print("\n");
  }
}
