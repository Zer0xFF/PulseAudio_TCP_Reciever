package net.madnation.pulseaudio;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Switch;

public class MainActivity extends AppCompatActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		final Switch stream_switch = findViewById(R.id.stream_switch);
		stream_switch.setOnClickListener(new View.OnClickListener()
		{
			@Override
			public void onClick(View view)
			{
				boolean startStream = stream_switch.isChecked();
				if(startStream)
				{
					int err = NativeLibs.Start();
					if(err != 0)
					{
						stream_switch.setChecked(false);
						//TODO print error based on code
					}
				}
				else
				{
					NativeLibs.Stop();
				}
			}
		});
	}
}
