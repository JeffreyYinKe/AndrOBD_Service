package com.fr3ts0n.ecu.gui.androbd;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.*;

import java.util.Set;

public class BtDeviceConnectService extends Service {
    private Messenger replyMessenger;
    String devPattern = "OBDLink\\s+MX.*\\s+\\d+";
    private BluetoothAdapter mBtAdapter;
    public static final String EXTRA_DEVICE_ADDRESS = "device_address";
    @Override
    public IBinder onBind(Intent intent) {
        replyMessenger = intent.getParcelableExtra("messenger");
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        replyMessenger = intent.getParcelableExtra("messenger");
        Message message = Message.obtain();

        String devAddress ="";
        // Get the local Bluetooth adapter
        mBtAdapter = BluetoothAdapter.getDefaultAdapter();

        // Get a set of currently paired devices
        Set<BluetoothDevice> pairedDevices = mBtAdapter.getBondedDevices();
        for (BluetoothDevice device : pairedDevices)
        {
            if ( device.getName().matches(devPattern)) {
                devAddress = device.getAddress();
                break;
            }
        }
        mBtAdapter.cancelDiscovery();

        Bundle bundle = new Bundle();
        bundle.putString(EXTRA_DEVICE_ADDRESS, devAddress);
        message.setData(bundle);
        try {
            replyMessenger.send(message);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

        return START_NOT_STICKY;
    }
}


