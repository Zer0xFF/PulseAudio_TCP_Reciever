package net.madnation.pulseaudio;

import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Switch;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity
{

	String PREFS_NAME = "SETTINGS";

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		final Switch stream_switch = findViewById(R.id.stream_switch);
		final TextView host_text = findViewById(R.id.host_text);
		final TextView port_text = findViewById(R.id.port_text);

		SharedPreferences prefs = getSharedPreferences(PREFS_NAME, MODE_PRIVATE);
		host_text.setText(prefs.getString("HOST", ""));
		port_text.setText(prefs.getString("PORT", ""));

		stream_switch.setOnClickListener(new View.OnClickListener()
		{
			@Override
			public void onClick(View view)
			{
				boolean startStream = stream_switch.isChecked();
				if(startStream)
				{
					int port = getInt(port_text.getText().toString());
					if(port < 0)
					{
						//TODO error out
						return;
					}
					int err = NativeLibs.Start(host_text.getText().toString(), port);
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

		Button save_button = findViewById(R.id.save_button);
		save_button.setOnClickListener(new View.OnClickListener()
		{
			@Override
			public void onClick(View view)
			{
				SharedPreferences.Editor editor = getSharedPreferences(PREFS_NAME, MODE_PRIVATE).edit();
				editor.putString("HOST", host_text.getText().toString());
				editor.putString("PORT", port_text.getText().toString());
				editor.apply();
			}
		});
	}

	int getInt(String int_string)
	{
		int default_val = -1;

		try
		{
			default_val = Integer.parseInt(int_string);
		}
		catch(NumberFormatException nfe)
		{
		}
		return default_val;
	}
}
