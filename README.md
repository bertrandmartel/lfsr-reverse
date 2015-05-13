# Linear Feedback Shift Register Algorithm Hack study

Reverse engineering issue involving :
* Linear Feedback Shift Register algorithms
* Endianness issues
* Brute force
* Some patience

<b>Problem subject</b>

I'm describing here a reverse engineering peronnal project about authentication of an embedded device to a smartphone software.

I managed to extract the following algorithm for generating authentication frame to embedded device :

``
	int i;
	for (i = 0; i < period; i++) {
		if((seed & 0x80000000) != 0) {
			seed = seed << 1^0xD0000001;
		} else {
			seed <<= 1;
		}
	}
``
This is a kind of linear Feedback Shift Register : 

> a linear-feedback shift register (LFSR) is a shift register whose input bit is a linear function of its previous state
<i>source : http://en.wikipedia.org/wiki/Linear_feedback_shift_register</i>

Here is some glossary : 
* seed   : initial vector used
* period : number of iteration to make 
* taps   : the bit position that affects the next state (here 0xD0000001)

<hr/>

<b>Problem data</b>

Here is how it works : 
* smartphone software send a REQUEST_AUTHENTICATION frame
* embedded device will send back a RESPONSE frame with 4 bytes data like that : 83 15 C7 05
* smartphone will receive this frame and compute these 4 bytes with a secret key and then compute LFSR algorithm I told you above
* smartphone will send the computed LFSR bytes composed of 8 bytes like that  : 7D 06 6C DC 38 70 50 D6

I have all the algorithm that is used to compute secret key with input bytes from emebedded device.

* secret key is 8 bytes : [ a , b , c , d , e , f ,g , h ] 
 
* data received from embedded device is a 4 Bytes vector : [ w , x , y , z ]

* this secret key is divided in 2 vectors [ a , b , c , d ] and [ e , f , g , h ]

Let's make variables :

A = [ a , b , c , d ]
B = [ e , f , g , h ]

a,b,c,d are Bytes (8 bits each)
A,B are integer (32 bits)

We have to compute 2 LFSR of 32 bits :

<b>first LFSR</b>
* seed          : A          4 Bytes
* period        : [ x , w ]  2 Bytes
* LFSR computed : C          4 Bytes

<b>second LFSR</b>
* seed          : B          4 Bytes
* period        : [ z , y ]  2 Bytes
* LFSR computed : D          4 Bytes

The following algorithm (the same as in the beginning) is computed for first and second LFSR
``
	int i;
	for (i = 0; i < period; i++) {
		if((seed & 0x80000000) != 0) {
			seed = seed << 1^0xD0000001;
		} else {
			seed <<= 1;
		}
	}
``

Ouput of both of them are C vector and D vector : that's 8 Bytes as a whole

Let's say that : 
C = [ a1 , a2 , a3 , a4 ]
D = [ b1 , b2 , b3 , b4 ]

These vectors are in Big endian, they will be converted into Little endian to be sent to embedded device 

here is the final vector sent to embedded device :

E = [ a4 , a3 , a2 , a1 , b4 , b3 , b2 , b1 ]

note : Bytes are sent from embedded device in Little endianness, I already reordered them in Big endian in vectors above

<b>The problem is following : I need to find the secret key [ a , b , c , d , e , f ,g , h ]  that takes part in computing the 2 LFSR sent to the emebedded device</b>

<hr/>

<b>Looking what's going on</b>

The good news is that output is sent to embedded device without any other secured transport layer and I can sniff all traffic and get LFSR output computation frame.

From that, I have following data :
* data sent from emebedded device (to compute LFSR with secret key )                : 83 15 C7 05
* LFSR output data (from computation and concatenation of both LFSR as shown above) : 7D 06 6C DC 38 70 50 D6

We have here to reverse process to set value for C and D (result of LFSR computations 1 and 2) :

E = [ 7D, 06, 6C, DC, 38, 70, 50, D6 ]
C = [ DC, 6C, 06, 7D ]
D = [ D6, 50, 70, 38 ]


We have also data from emebedded device [ w , x , y , z ] which give [ 83, 15, C7, 05 ]

That makes value of both period of LFSR 1 and 2 computations :

period of first LFSR  = [ x , w ] = [ 15, 83 ]
period of second LFSR = [ z , y ] = [ 05, C7 ]


So we have to find seed values of both LFSR to get to secret key ! 

<b>first LFSR</b>
* seed          : A                  4 Bytes
* period        : [ 15, 83 ]         2 Bytes
* LFSR computed : [ DC, 6C, 06, 7D ] 4 Bytes

<b>second LFSR</b>
* seed          : B                  4 Bytes
* period        : [ 05, C7 ]         2 Bytes
* LFSR computed : [ D6, 50, 70, 38 ] 4 Bytes

We have to find A and B and we know algorithm that computes it with period to get [ computed LFSR ]

<hr/>

<b>How to find respective seeds of both LFSR ? </b>

LFSR algorithm cant be reversed easily as bit shifting to the right (or to the left as this is occuring here) compels to make assumption of LSB value (or MSB value here for a left bit shifting). This is very annoying since iteration period is very high (always >1000) which makes A LOT of possibilities.
Basically with first LFSR we have to make assumption of bit shifting for each iteration and we have 5507 iterations => that makes 2^5507 possibilities => we cant do that.

Good news is that seed are integer on 32 bits which makes 4 294 967 296 possibilities that will computes the LFSR algorithm shown previously. This scenario seems realistic.We can do Brute force to find the key.

In case Brute force is not enough, and we find out there is something else in computation and that seed is generated randomly, we could try to use some algebra research to make this computation more quickly.

<hr/>

<b>Brute force method</b>

You will find in ``lfsr_reverse.c`` brute force method I used to find A and B vectors from computation result and specified period values.
You can compile it with : 

``gcc  lfsr_reverse.c -lpthread -o lfsr_reverse``

I parameterized 30 threads launched on 4 physical cores (8 logicals) to maximize parallelization of tasks and computing time.

It took more than 8 hour to find both values which seems fair (but still).

Typical computation per thread is : 

``
int begin = 0x80000000+ITERATION_AVERAGE_PER_THREAD*threadId;
int end = ITERATION_MAX;

for (i = begin;i< end;i++)
{
	int result = encoder32bitLFSR(i,period2,filter);
	if (result == C2)
	{
		printf("Found the solution for C => %d\n",i);
		return NULL;
	}
}
``

At the end we found : 

A = [ a , b , c , d ] = [ 42 , C0 , 80 , 50 ]

B = [ e , f , g , h ] = [ 44 , 92 , 1C , 69 ]

And we deduct secret key : [ a , b , c , d , e , f ,g , h ]

Java Eclipse project here features some vector testing just to be sure secret id has been well calculated.

Brute force program here is likely not to be optimized for such a process.

<b>Algebra research</b>

This is a testing section for further research on LFSR processus and reverse algebra methods.

Test with seed : E7 8F 31 5D

      E     7    8    F    3    1    5    D
=>    1 110 0111 1000 1111 0011 0001 0101 1101
 
i=0   1 100 1111 0001 1110 0110 0010 1011 1010  << 1
=>    0 001 1111 0001 1110 0110 0010 1011 1010  XOR
 
i=1   0 011 1110 0011 1100 1100 0101 0111 0100  << 1
i=2   0 111 1100 0111 1001 1000 1010 1110 1000  << 1
i=3   1 111 1000 1111 0011 0001 0101 1101 0000  << 1

Described LFSR is a Galois LFSR with a bit shiting to the left ( sort of Big Endian to Little Endian Gallois LFSR).

The arithemtic polynom for this LFSR is : x^32 + x^31 + x^29 + x + 1 (coeff ar 1s and 0s)


To be continued ......

<b>Conclusion</b>

Secret key is found ! We can now authenticate on the embedded device

<b>Some links that helped me</b>

http://cryptography.wikia.com/wiki/Linear_feedback_shift_register
http://en.wikipedia.org/wiki/Linear_feedback_shift_register


