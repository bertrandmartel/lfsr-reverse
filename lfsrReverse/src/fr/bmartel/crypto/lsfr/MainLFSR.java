package fr.bmartel.crypto.lsfr;

/**
 * Testing functions to check brute force result is OK
 * 
 * @author Bertrand Martel
 *
 */
public class MainLFSR {

	public static void main(String[] args) {
		
		//filter used in software algorithm
        int filter = 0xD0000001;
        
        //value of period are sent fom embedded device
        int period1 = 0x05 << 8 | 0xC7; // B LFSR period
        int period2 = 0x15 << 8 | 0x83; // A LFSR period
        
        // value of B and A found by brute force !
        int B = 0x44921C69; //1A5C
        int A = 0x42C08050; //0A31
        
        //expected LFSR1 and LFSR2
        int expectedResult1 = 0xD6507038;
        int expectedResult2 = 0xDC6C067D;
        
        boolean verify1 =expectedResult1==Lfsr.encoder32bitLFSR(B, period1, filter);
        boolean verify2 =expectedResult2==Lfsr.encoder32bitLFSR(A, period2, filter);
        
        System.out.println("B is verified => " + verify1);
        System.out.println("A is verified => " + verify2);
        
        //value of period are sent fom embedded device
        int period11 = 0x13 << 8 | 0xD0; // B LFSR period
        int period22 = 0x04 << 8 | 0x1A; // A LFSR period
        
        System.out.println("A => " + Lfsr.encoder32bitLFSR(A, period11, filter));
        System.out.println("B => " + Lfsr.encoder32bitLFSR(B, period22, filter));
        
	}
	
}

