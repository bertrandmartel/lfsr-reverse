package fr.bmartel.crypto.lsfr;

/**
 * Lfsr 32 bit custom algorithm
 * 
 * @author Bertrand Martel
 *
 */
public class Lfsr {

	/**
	 * Lfsr 32 bit custom algorithm (integer)
	 * 
	 * @param seed
	 * 		input of lfsr algo
	 * @param period
	 * 		iteration number
	 * @param filter
	 * 		polynomial used
	 * @return
	 * 		lfsr encoding result
	 */
	public static int encoder32bitLFSR(int seed,int period,int filter)
	{
        for (int i = 0; i < period; i++) {
            if((seed & 0x80000000) != 0) {
            	seed = seed << 1^filter;
            } else {
            	seed <<= 1;
            }
        }
        return seed;
	}
	
	/**
	 * Print integer in bit format with spaces : XXXX XXXX XXXX XXXX XXXX XXXX XXXX XXXX XXXX
	 * 
	 * @param index
	 * 		iteration number
	 * @param seed
	 * 		current iteration seed value
	 */
	public static void printnLFSR(int index,int seed)
	{
		String binary = Integer.toBinaryString(seed);
		
		for (int i =binary.length();i< 32;i++)
		{
			binary="0"+binary;
		}
		
		if (index>=0)
			System.out.println("i=" + index);
		
		for (int i = 0; i < 8;i++)
		{
			if (i==0)
				System.out.print("\t\t" + binary.substring(i*4, i*4+4) + " ");
			else
				System.out.print(binary.substring(i*4, i*4+4) + " ");
		}
		System.out.println("");
	}
}
