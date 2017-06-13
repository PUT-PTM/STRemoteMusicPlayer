//
//  SerialViewController.swift
//  HM10 Serial
//


import UIKit
import CoreBluetooth
import QuartzCore

var changedSong = false
var bufor = String()

final class SerialViewController: UIViewController, UITextFieldDelegate, BluetoothSerialDelegate {

//MARK: IBOutlets and Variables
    
    
    
    
    
    @IBOutlet weak var volume: UISlider!
    @IBOutlet weak var songTitle: UILabel!
    @IBOutlet weak var songList: UIButton!
    @IBOutlet weak var playButton: UIButton!
    @IBOutlet weak var barButton: UIBarButtonItem!
    @IBOutlet weak var navItem: UINavigationItem!
    


//MARK: Functions
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // init serial
        serial = BluetoothSerial(delegate: self)
        
        volume.isContinuous = false
        volume.isEnabled = false
        
        reloadView()
        
        songList.isEnabled = false
        
        NotificationCenter.default.addObserver(self, selector: #selector(SerialViewController.reloadView), name: NSNotification.Name(rawValue: "reloadStartViewController"), object: nil)
        
    }

    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        changeSong()
        
    }
    
    func changeSong() {
        if changedSong {
            let selected = songs[selectedSong]
            serial.sendMessageToDevice("\(selectedSong)s")
            songTitle.text = selected
        }
        changedSong = false
    }
    
    
    func reloadView() {
        // in case we're the visible view again
        serial.delegate = self
        
        if serial.isReady {
            navItem.title = serial.connectedPeripheral!.name
            barButton.title = "Disconnect"
            barButton.tintColor = UIColor.red
            barButton.isEnabled = true
        } else if serial.centralManager.state == .poweredOn {
            navItem.title = "Bluetooth Player"
            barButton.title = "Connect"
            barButton.tintColor = view.tintColor
            barButton.isEnabled = true
        } else {
            navItem.title = "Bluetooth Player"
            barButton.title = "Connect"
            barButton.tintColor = view.tintColor
            barButton.isEnabled = false
        }
    }
    

//MARK: BluetoothSerialDelegate
    
    func serialDidReceiveString(_ message: String) {
        if message == "?" {
            songs.append(bufor)
            bufor = ""
        } else if message == "*" {
            songs.append(bufor)
            bufor = ""
            songList.isEnabled = true
            volume.isEnabled = true
        } else {
            bufor.append(message)
        }
    }
    
    func serialDidDisconnect(_ peripheral: CBPeripheral, error: NSError?) {
        reloadView()
        let hud = MBProgressHUD.showAdded(to: view, animated: true)
        hud?.mode = MBProgressHUDMode.text
        hud?.labelText = "Disconnected"
        hud?.hide(true, afterDelay: 1.0)
        songList.isEnabled = false
        volume.isEnabled = false
        volume.value = 50.0
        songTitle.text = ""
    }
    
    func serialDidChangeState() {
        reloadView()
        if serial.centralManager.state != .poweredOn {
            let hud = MBProgressHUD.showAdded(to: view, animated: true)
            hud?.mode = MBProgressHUDMode.text
            hud?.labelText = "Bluetooth turned off"
            hud?.hide(true, afterDelay: 1.0)
            songList.isEnabled = false
            volume.isEnabled = false
            volume.value = 50.0
            songTitle.text = ""
        }
    }
    
    
    
    
//MARK: IBActions

    @IBAction func barButtonPressed(_ sender: AnyObject) {
        if serial.connectedPeripheral == nil {
            performSegue(withIdentifier: "ShowScanner", sender: self)
        } else {
            serial.disconnect()
            reloadView()
        }
    }
    
    
    @IBAction func play(_ sender: Any) {
        
        if !serial.isReady {
            let alert = UIAlertController(title: "Not connected", message: "What am I supposed to send this to?", preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.default, handler: { action -> Void in self.dismiss(animated: true, completion: nil) }))
            present(alert, animated: true, completion: nil)
            return
        }
        
        serial.sendMessageToDevice("r")
        
    }
    
    
    
    @IBAction func previous(_ sender: Any) {
        
        if !serial.isReady {
            let alert = UIAlertController(title: "Not connected", message: "What am I supposed to send this to?", preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.default, handler: { action -> Void in self.dismiss(animated: true, completion: nil) }))
            present(alert, animated: true, completion: nil)
            return
        }

        
        if !songs.isEmpty {
            if selectedSong == 0 {
                songTitle.text = songs[songs.count-1]
                selectedSong = songs.count - 1
            } else {
                songTitle.text = songs[selectedSong-1]
                selectedSong -= 1
            }
        }
        
        serial.sendMessageToDevice("p")
    }
    
    
    
    @IBAction func next(_ sender: Any) {
        
        if !serial.isReady {
            let alert = UIAlertController(title: "Not connected", message: "What am I supposed to send this to?", preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "Dismiss", style: UIAlertActionStyle.default, handler: { action -> Void in self.dismiss(animated: true, completion: nil) }))
            present(alert, animated: true, completion: nil)
            return
        }
        
        if !songs.isEmpty {
            if selectedSong == songs.count-1 {
                songTitle.text = songs[0]
                selectedSong = 0
            } else {
                songTitle.text = songs[selectedSong+1]
                selectedSong += 1
            }
        }
        
        serial.sendMessageToDevice("n")
    }
    
    @IBAction func volumeChanged(_ sender: Any) {
        serial.sendMessageToDevice(String(Int(volume.value))+"v")
    }
    
    
}
