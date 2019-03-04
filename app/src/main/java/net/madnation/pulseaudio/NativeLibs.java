package net.madnation.pulseaudio;

public class NativeLibs
{
	static
	{
		System.loadLibrary("native-lib");
	}

	public static native int Start(String text, int port);

	public static native void Stop();

}