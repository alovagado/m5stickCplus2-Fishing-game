Enhanced Fishing Game for M5StickC Plus 2

Download Instructions: Click the download button (📥) in the top-right corner of this document to save the README.md file to your computer.

An immersive fishing simulation game designed for the M5StickC Plus 2, featuring realistic physics, diverse fish species, weather effects, and engaging gameplay mechanics.
🎮 Game Features
Core Gameplay

Motion-Controlled Fishing: Tilt the device to aim your fishing rod and swing to cast
Realistic Fishing Mechanics: Hold Button B to extend your line, release to set the hook
12 Unique Fish Species: Each with different behaviors, point values, and catch difficulties
Dynamic Weather System: Clear, rainy, and stormy conditions that affect gameplay
Combo System: Chain catches for bonus points and multipliers

Advanced Features

Power-Up System: Temporary boosts including Big Hook, Time Slow, and Double Points
Particle Effects: Splash effects, bubbles, and visual feedback for enhanced immersion
Adaptive Difficulty: Game becomes more challenging as you progress
High Score Tracking: Compete against your personal best
Sound & Vibration: Audio-visual feedback for all game events

🐟 Fish Types
Fish TypePointsDifficultySpecial BehaviorNormal10-20EasyStandard swimming patternsFast25-30MediumQuick, erratic movements with motion blurRare50HardSine wave swimming patterns, purple indicatorGolden100Very HardShimmering effect, highest point value
🎯 Controls
Button Layout

Button A: Start game / Menu navigation / Restart
Button B:

Hold to extend fishing line
Tap rapidly when fish is hooked
Toggle settings in menu



Motion Controls

Tilt Left/Right: Aim fishing rod (±30° range)
Swing Motion: Cast your line (uses accelerometer and gyroscope)
IMU Sensitivity: Responds to acceleration changes > 2.0G or movement > 0.8G

🌦️ Weather System
The game features a dynamic weather system that changes every 30 seconds:

Clear Weather ☀️: Normal fishing conditions
Rainy Weather 🌧️: Animated raindrops, slightly affects fish behavior
Stormy Weather ⛈️: Turbulent conditions, fish move more erratically

🎮 Game States & Flow
Menu → Waiting to Cast → Fishing → Fish Hooked → Caught/Missed
  ↑                                      ↓
  └─────────────── Game Over ←──────────┘
Detailed State Descriptions

Menu: Game introduction and settings
Waiting to Cast: Aim your rod and swing to start
Fishing: Control line depth, avoid going too deep
Fish Hooked: Rapidly tap Button B to reel in the fish
Caught/Missed: Success celebration or missed opportunity
Game Over: Final score and restart option

🏆 Scoring System
Base Points

Normal Fish: 10-20 points
Fast Fish: 25-30 points
Rare Fish: 50 points
Golden Fish: 100 points

Bonus Multipliers

Combo Bonus: +5 points per consecutive catch
Power-Up Bonus: 2x points when Double Points is active
Difficulty Bonus: Additional points based on game progression

⚡ Power-Up System
Power-ups spawn randomly during gameplay and last for 10 seconds:

Big Hook 🪝: Increases hook collision radius for easier catches
Time Slow ⏰: Reduces line extension speed for better control
Double Points 💰: Doubles all point values temporarily

🛠️ Technical Requirements
Hardware

M5StickC Plus 2 device
Built-in IMU (accelerometer/gyroscope)
135x240 color LCD display
Two programmable buttons (A & B)

Software Dependencies
cpp#include <M5StickCPlus2.h>
Memory Usage

Fish Array: 12 fish objects with position, velocity, and state data
Particle System: 20 particle objects for visual effects
Weather System: Rain drop array and weather state tracking
Display Optimization: Reduced refresh rates to minimize screen flashing

🔧 Installation & Setup

Install Arduino IDE with M5StickC Plus 2 board support
Download the Code: Copy Fishing_game_v2.ino to your Arduino projects folder
Connect Device: Use USB-C cable to connect M5StickC Plus 2
Upload: Select correct board and port, then upload the sketch
Play: Device will boot directly into the game menu

Required Libraries

M5StickCPlus2 library (install via Arduino Library Manager)

🎨 Display Optimization
The game implements several optimization techniques:

Selective Rendering: Only redraws changed screen areas
Frame Rate Control: 50ms full updates, 20ms partial updates
Motion Detection: Tracks fish and hook movement to minimize unnecessary redraws
Status Caching: Reduces text rendering when status hasn't changed

🔊 Audio & Haptic Feedback
Sound Effects

Casting splash: 800Hz tone
Fish hooked: 1000Hz tone (varies by fish type)
Golden fish: 1800Hz tone
Power-up spawn: 1500Hz tone
Game start: 1000Hz tone

Vibration Patterns

Successful cast: 50ms
Fish hooked: 75-150ms (varies by fish type)
Missed turn: 200ms
Fish caught: 100ms

🐛 Troubleshooting
Common Issues
Game not responding to motion

Ensure device is held upright during calibration
Check that IMU initialization completed successfully

Screen flickering

Normal during intense particle effects
Reduce particle count if performance issues occur

Fish not appearing

Check if fish array initialization completed
Restart game from menu if fish spawn incorrectly

Sound/vibration not working

Toggle settings in menu (Button B)
Check hardware capabilities of your specific device

🚀 Future Enhancements
Potential additions for future versions:

Multiplayer Mode: Compete with other M5StickC devices
Achievement System: Unlock rewards for specific accomplishments
Customizable Bait: Different lures affect fish attraction
Day/Night Cycle: Time-based lighting and fish behavior changes
Save Game Progress: Persistent statistics and unlockables

📝 Code Structure
Key Components

Game State Machine: Handles transitions between game phases
Physics Engine: Realistic fish movement and collision detection
Particle System: Visual effects for enhanced gameplay experience
Weather Engine: Dynamic environmental conditions
Display Manager: Optimized rendering with selective updates

Performance Features

Efficient memory usage with static arrays
Reduced CPU load through smart update scheduling
Optimized collision detection algorithms
Minimal heap allocation during gameplay

📄 License
This project is open source and available under the MIT License. Feel free to modify, distribute, and enhance the game for your own projects.
🤝 Contributing
Contributions are welcome! Areas for improvement:

Additional fish species and behaviors
Enhanced visual effects
Performance optimizations
New power-up types
Advanced weather patterns
