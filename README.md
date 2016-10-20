# pebble-darts-counter
Darts game counter for pebble smartwatch

## Screenshots

![Game menu](https://raw.githubusercontent.com/vassdoki/pebble-darts-counter/master/pebble_screenshot_a_menu.png)
![Game screen 1](https://raw.githubusercontent.com/vassdoki/pebble-darts-counter/master/pebble_screenshot_b_throws.png)
![Game screen 2](https://raw.githubusercontent.com/vassdoki/pebble-darts-counter/master/pebble_screenshot_c_wrong_throw.png)
![Game screen 3](https://raw.githubusercontent.com/vassdoki/pebble-darts-counter/master/pebble_screenshot_wrong_double_out.png)


## Usage

Only x01 games are implemented. You can set the game parameters:

 * Number of players
 * Game length (301, 501, 701)
 * Double in
 * Double out

### Entering throws

The up button adds 5, down button adds 1. Press select to enter the score.

Double, tripple: hold the select button and release it when you have the
right setting: double, tripple, cancel, cancel round. Cancel will erase the current throw. Cancel round will
erase all throws in the current round.

Shortcuts: to speed up entering large numbers, you can hold down up button for 19.
Hold down down button for 16.

## Example throw entering

* 3: down, down, down
* double 2: down, down, hold select
* bull: hold up, up, up
* 15: up, up, up
* 16: hold down
* 17: hold down, down
* 18: hold down, down, down
* 19: hold up
* 20: hold up, up

Finish with one select button.

# Developer help

## X01 gui functions

![Game screen 2](https://raw.githubusercontent.com/vassdoki/pebble-darts-counter/master/x01_gui_h_guide.png)
