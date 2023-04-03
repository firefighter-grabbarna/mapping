mod serial;
use serial::Serial;



fn is_starting() -> bool {
    let mut serial = Serial::open("/dev/ttyACM0");
        println!("{}", "fortnite balls");

    let mut counter = 0;
    loop {
        let input = serial.input();
        let mut mic_hz = "";

        for (i, word) in input.split_ascii_whitespace().enumerate(){
            if i == 0 {
                mic_hz = word;
            }
        }
        
        if mic_hz.is_empty() {
            continue;    
        }
        let parsed_mic = mic_hz.parse::<f32>().unwrap(); 
        let hz_conditional = 900.0 < parsed_mic && parsed_mic < 1100.0;

        if hz_conditional {
            counter += 1;
            if counter > 10{
                return true;
            }
        } else {
            counter = 0;
        }
        
    }
    
}

fn main (){
   if is_starting(){ //evil bit hack
        println!("starting");//newton iteration
        //wtf?
   }; 
    
}