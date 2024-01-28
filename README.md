# Blueprint Screenshot Tool

## Description
The "Blueprint Screenshot Tool" is a simple Unreal Engine plugin that adds a "Take Screenshot" button in various editors like Blueprint, Material, and more. This feature allows users to capture blueprint graphs effortlessly, no matter how large they are.

## How to install
1. Download [latest release](https://github.com/Gradess2019/BlueprintScreenshotTool/releases/latest) for your Unreal Engine version
2. Unzip into: **\<ProjectDirectory\>/Plugins** (create Plugins directory if it doesn't exist)
3. If you are using C++: Right Mouse Button on your **.uproject** file -> Generate Visual Studio project files
4. Launch project
5. If it's not enabled: Go to Edit -> Plugins -> "Project" category -> Editor -> Enable "Blueprint Screenshot Tool" and restart the editor
7. Done

## How to use
1. Open any editor that has graphs (Blueprint, Material, etc.)
2. You should see a new button in the toolbar called "Take Screenshot"

    ![image](https://github.com/Gradess2019/BlueprintScreenshotTool/assets/38568823/fb60bc48-339b-42cc-99a7-4d56810e5603)

3. Click it or press **Ctrl+F7** and wait for the screenshot of the graph to be taken

    **Tip:** You can take screenshot of specific nodes by selecting them before taking the screenshot

4. You'll see a notification when the screenshot is ready with a hyperlink to the screenshot file
5. Click the hyperlink to open the screenshot directory
6. Done

## Hotkeys
* **Ctrl+F7** - Take screenshot
* **Ctrl+F8** - Open screenshot directory

## Configuration
1. Go to Edit -> Editor Preferences -> "Plugins" category -> "Blueprint Screenshot Tool" section
2. You should see the following settings:

    ![image](https://github.com/Gradess2019/BlueprintScreenshotTool/assets/38568823/d9bbbfec-7c80-4094-8902-2d8cf239080b)

3. Adjust the settings as needed to match your preferences and project requirements.
4. Done