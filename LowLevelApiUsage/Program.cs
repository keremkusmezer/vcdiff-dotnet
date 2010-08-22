using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LowLevelApiUsage
{
    class Program
    {
        static void Main(string[] args)
        {
            EncodingExample();
            DecodingExample();
        }

        static void EncodingExample()
        {
            byte[] sourceFile = new byte[] { };
            List<byte> result = new List<byte>();

            using (VCDiffDotNet.LowLevel.Encoder encoder = new VCDiffDotNet.LowLevel.Encoder())
            {
                encoder.InitEncoder(sourceFile, false, false, false, false);

                byte[] partialOutput;
                encoder.StartEncoding(out partialOutput);
                result.AddRange(partialOutput);

                byte[] partOfTargetFile = new byte[] { };
                encoder.EncodeChunk(partOfTargetFile, out partialOutput);
                result.AddRange(partialOutput);

                encoder.FinishEncoding(out partialOutput);
                result.AddRange(partialOutput);
            }

            byte[] patchFile = result.ToArray();
        }

        static void DecodingExample()
        {
            byte[] sourceFile = new byte[] { };
            List<byte> result = new List<byte>();

            using (VCDiffDotNet.LowLevel.Decoder decoder = new VCDiffDotNet.LowLevel.Decoder())
            {
                int maxSize = 1 << 24;
                decoder.InitDecoder(sourceFile, true, maxSize, maxSize);
                decoder.StartDecoding();

                byte[] partialOutput;
                byte[] partOfPatchFile = new byte[] { };
                decoder.DecodeChunk(partOfPatchFile, out partialOutput);
                result.AddRange(partialOutput);

                decoder.FinishDecoding();
            }

            byte[] targetFile = result.ToArray();
        }
    }
}