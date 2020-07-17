using System;
using System.Text;
using System.Security.Cryptography;
using System.Collections.Generic;

public class MakeGuid {

	public Guid GetProviderGuid(String providerName)
	{
		if (providerName == null) throw new ArgumentNullException("providerName");
		string name = providerName.ToUpperInvariant();
		byte[] buffer = new byte[(name.Length * 2) + 0x10];
		uint num  = 0x482c2db2;
		uint num2 = 0xc39047c8;
		uint num3 = 0x87f81a15;
		uint num4 = 0xbfc130fb;
		for (int i = 3; 0 <= i; i--)
		{
			buffer[i] = (byte)num;
			num = num >> 8;
			buffer[i + 4] = (byte)num2;
			num2 = num2 >> 8;
			buffer[i + 8] = (byte)num3;
			num3 = num3 >> 8;
			buffer[i + 12] = (byte)num4;
			num4 = num4 >> 8;
		}

		for (int j = 0; j < name.Length; j++)
		{
			buffer[((2 * j) + 0x10) + 1] = (byte)name[j];
			buffer[(2 * j) + 0x10] = (byte)(name[j] >> 8);
		}

		SHA1 sha = SHA1.Create();
		byte[] buffer2 = sha.ComputeHash(buffer);
		int a = (((((buffer2[3] << 8) + buffer2[2]) << 8) + buffer2[1]) << 8) + buffer2[0];
		short b = (short)((buffer2[5] << 8) + buffer2[4]);
		short num9 = (short)((buffer2[7] << 8) + buffer2[6]);
		return new Guid(a, b, (short)((num9 & 0xfff) | 0x5000), buffer2[8], buffer2[9], buffer2[10], buffer2[11], buffer2[12], buffer2[13], buffer2[14], buffer2[15]);
	}

}
