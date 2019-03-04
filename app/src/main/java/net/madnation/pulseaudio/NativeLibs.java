package net.madnation.pulseaudio;

public class NativeLibs
{
	static
	{
		System.loadLibrary("native-lib");
	}

	public static native int Start();

	public static native void Stop();

}