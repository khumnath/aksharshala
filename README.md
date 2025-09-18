
# Aksharshala - Typing Tutor

Aksharshala is a modern, feature-rich typing tutor alternative to **TYPESHALA** designed to help users improve their typing speed and accuracy in both English and Nepali (Traditional). It provides real-time feedback, user-specific progress tracking, and an interactive learning environment.

## ✨ Features

-   **Multi-Language Support**: Practice typing in both **English** and **Nepali (Traditional)** layouts.
    
-   **User Profiles**: Create and switch between multiple user profiles to track individual progress.
    
-   **Comprehensive Statistics**: Get detailed performance reports including Words Per Minute (WPM), accuracy, and characters per minute. All test results are saved for historical review.
    
-   **Structured Lessons**: Progress through curated lesson groups designed to build muscle memory, from the home row to more complex sentences.
    
-   **Interactive On-Screen Keyboard**: A visual keyboard that highlights the next key and provides instant feedback for correct and incorrect presses.
    
-   **Finger Guidance**: Reinforce proper touch-typing technique by showing which finger to use for each key.
    
-   **Custom Practice Text**: Not interested in a lesson? You can paste your own text to practice with.
    
-   **Light & Dark Themes**: Switch between light and dark modes for your comfort.
    
-   **Real-time Feedback**: Characters are colored instantly as you type, letting you know if you've made a mistake.
    

## 🛠️ Build Instructions (Source-Based)

### ✅ Prerequisites

Install Qt6 development packages:

#### Debian / Ubuntu

```
sudo apt install qt6-base-dev qt6-webengine-dev qt6-websockets-dev  qt6-tools-dev qt6-base-dev-tools

```

#### Arch Linux

```
sudo pacman -S qt6-base qt6-webengine qt6-websockets

```

#### Fedora

```
sudo dnf install qt6-qtbase-devel qt6-qtwebengine-devel qt6-qtwebsockets-devel

```

### 🧱 Build Steps (CMake)

```
git clone https://github.com/khumnath/aksharshala.git
cd aksharshala
mkdir build && cd build
cmake ..
make -j$(nproc)

```

This will produce the `aksharshala` binary in the `build/` directory.

## 📦 Runtime Dependencies

If you're running a prebuilt binary, these are the **runtime packages** required:

### Debian / Ubuntu

```
sudo apt install qt6-base-plugins qt6-webengine-plugins qt6-websockets-plugins libqt6core6 libqt6gui6 libqt6widgets6 libqt6webengine6 libqt6websockets6 libqt6network6

```

### Arch Linux

```
sudo pacman -S qt6-base qt6-webengine qt6-websockets

```

Arch bundles runtime and dev files together, so no separate runtime packages.

### Fedora

```
sudo dnf install qt6-qtbase qt6-qtwebengine qt6-qtwebsockets

```

## 🤝 Contributing

Contributions are welcome! If you'd like to help improve Aksharshala, please feel free to fork the repository and submit a pull request. For major changes, please open an issue first to discuss what you would like to change.

## 🐞 Reporting Issues & Suggestions

Found a bug or have an idea for a new feature? We'd love to hear from you! Please open an issue on our GitHub issue tracker and provide as much detail as possible, including:

-   A clear and descriptive title.
    
-   Steps to reproduce the bug.
    
-   Expected behavior vs. actual behavior.
    
-   Screenshots, if applicable.
    

## 📜 License

The Aksharshala project has a dual-license structure:

-   **Qt/C++ Application Wrapper**: The native desktop wrapper built with Qt/C++ is licensed under the **GNU General Public License v3.0 (GPLv3)**. You are free to use, modify, and distribute it under the terms of that license.
    
-   **Web Application Core:** The React-based web application (including TypeScript, CSS, and related assets) is proprietary software licensed exclusively to [Khumnath](https://khumnath.com.np). Redistribution, modification, or reuse of this code outside the official Aksharshala desktop application is prohibited without prior written consent.
