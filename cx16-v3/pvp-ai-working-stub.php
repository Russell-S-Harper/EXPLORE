<?php

/*
    Contents: "PVP-AI working stub" PHP file (c) 2026
  Repository: https://github.com/Russell-S-Harper/EXPLORE
     Contact: russell.s.harper@gmail.com
*/

// Relevant to PVP-AI
define('PLAYER_INDEX', 0);
define('FB_CUTOFF', 114);
define('LR_CUTOFF', 114);
define('DX_CUTOFF', 171);
define('DY_CUTOFF', 171);
define('DZ_CUTOFF', 191);
define('DZ_BIAS', 8);
define('PROB_LO', 0);
define('PROB_HI', 95);
define('K1', 3.755E08);
define('K2', 3.949E06);
define('CHT_EOL', chr(0));

// Relevant to the remote AI
define('API_URLS', array('chatgpt' => 'https://api.openai.com/v1/chat/completions',
			'gemini' => '',
			'deepseek' => ''));
define('API_KEYS', array('chatgpt' => 'sk-proj-...',
			'gemini' => '',
			'deepseek' => ''));

// Relevant to this file
define('NOTES', 'pvp-ai-notes.txt');
define('SCORES', 'pvp-ai-scores.txt');

date_default_timezone_set('UTC');

setlocale(LC_CTYPE, 'C.UTF-8');

$controls = array(
	'9' => 'version',
	'8' => 'player',
	'7' => 'event',
	'6' => 'score',
	'5' => 'eot',
	'0' => 'quit'
);

$versions = array(
	1000 => array(
		'A' => array('active', 'boolean', 'player %d joined game %d'),
		'B' => array('npc', 'boolean', 'player %d set new arena %d'),
		'C' => array('airborne', 'boolean', 'focus switched to player %d'),
		'D' => array('exploding', 'boolean', 'player %d damaged by player %d'),
		'E' => array('firing', 'boolean', 'player %d cornered or not moving!'),
		'F' => array('health', 'integer', 'player %d impeded: severity %d'),
		'G' => array('z_delta', 'integer', 'player %d reached top'),
		'H' => array('last_z_delta', 'integer', 'player %d reached bottom'),
		'I' => array('a_delta', 'integer', 'player %d advanced by player %d'),
		'J' => array('last_a_delta', 'integer', 'player %d eliminated by player %d'),
		'K' => array('level', 'integer', 'player %d won'),
		'L' => array('gear', 'integer'),
		'M' => array('identifier', 'integer'),
		'N' => array('target', 'integer'),
		'O' => array('joy', 'integer'),
		'P' => array('mss_delta', 'integer'),
		'Q' => array('damage', 'integer'),
		'R' => array('hit_cd', 'integer'),
		'S' => array('loading_cd', 'integer'),
		'T' => array('live_cd', 'integer'),
		'U' => array('x', 'integer'),
		'V' => array('y', 'integer'),
		'W' => array('z', 'integer'),
		'X' => array('angle', 'float09'),
		'Y' => array('sin', 'float12'),
		'Z' => array('cos', 'float12')
	)
);

$substitutions = array (
	array(
		'game 0',
		'game 1',
		'game 2',
		'severity 61',
		'severity 59',
		'severity 56',
		'severity 51'
	),
	array(
		'game (normal)',
		'game (as primary target)',
		'game (no refresh on last level)',
		'missile cornered',
		'blocked by wall',
		'intentionally idle (penalty possibly incurred)',
		'failure in pursuit'
	)
);

main('chatgpt', $controls, $versions, $substitutions);

function main($llm, $controls, $versions, $substitutions) {
	$socket = '/tmp/via2.sock';
	@unlink($socket);
	// Create server socket
	if (($server = socket_create(AF_UNIX, SOCK_STREAM, 0)) !== false
	    && socket_bind($server, $socket)
	    && socket_listen($server)) {
		// Launch the emulator after the socket is ready
		exec('./x16emu -scale 2 -mhz 8 -via2 -via2-socket '.escapeshellarg($socket).' -prg PVP-AI.PRG -run -gif pvp-ai.gif,wait > /dev/null 2>&1 &');
		if (($client = socket_accept($server)) !== false) {
			// Will need these
			$lrExtra = array('left', 'right');
			$adExtra = array('ascend', 'descend');
			$shipSpeed = array('slow', 'fast');
			$shipDelta = array('almost as fast as', 'as fast as', 'faster than');
			// Processing data
			$version = array();
			$players = array();
			$events = '';
			$scores = '';
			$working = array();
			$continue = true;
			$waiting = false;
			$ready = false;
			// Notes from previous game
			$notes = '(none)';
			$contents = @file(NOTES, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
			if ($contents)
				$notes = json_decode($contents[count($contents) - 1]);
			while ($continue) {
				$text = '';
				while (true) {
					$char = '';
					if (socket_recv($client, $char, 1, 0) && $char != CHT_EOL)
						$text .= $char;
					if ($char == CHT_EOL)
						break;
				}
				$key = @$text[0];
				$value = @intval(substr($text, 1));
				if (array_key_exists($key, $controls)) {
					switch ($controls[$key]) {
						case 'version':
							$version = $versions[$value];
							break;
						case 'eot':
							$ready = true;
						case 'player':
							if ($working) {
								$players[] = $working;
								$working = array();
							}
							break;
						case 'event':
							$event = chr(($value >> 10) + ord('A'));
							$player = ($value >> 6) & 15;
							$extra = $value & 63;
							if ($player == PLAYER_INDEX || (in_array($event, array('D', 'I', 'J')) && $extra == PLAYER_INDEX)) {
								$text = sprintf(@$version[$event][2] ?: "?", $player, $extra);
								$events .= '- '.str_replace($substitutions[0], $substitutions[1], $text).PHP_EOL;
							}
							break;
						case 'score':
							$player = $value >> 9;
							$winner = ($value >> 8) & 1;
							$score = (($value & 255) - 128) * 5;
							$scores .= ' '.$player.'  '.($winner? 'Y': 'N').'   '.substr('   '.$score, -4).PHP_EOL;
							break;
						case 'quit':
							$continue = false;
							break;
					}
				} else if (array_key_exists($key, $version)) {
					switch ($version[$key][1]) {
						case 'boolean':
							$working[$version[$key][0]] = !!$value;
							break;
						case 'integer':
							$working[$version[$key][0]] = $value;						
							break;
						case 'float09':
							// Used for angles
							$working[$version[$key][0]] = $value * 0.703125;				
							break;
						case 'float12':
							// Used for sin and cos
							$working[$version[$key][0]] = $value / 4096.0;						
							break;
					}
				}
				if ($ready) {
					// Set up actions and SWOT
					$actions = '';
					$swot = '';
					if (@$players[PLAYER_INDEX]['npc']) {
						$actions = '# Remote AI: Pick an action'.PHP_EOL.
							PHP_EOL.
							'Choose one option (E, F, G) and return it exactly.'.PHP_EOL.
							PHP_EOL.
							'E = Join normal game (recommended)'.PHP_EOL.
							'F = Join game as a primary target (hard)'.PHP_EOL.
							'G = Join game with no refresh on last level (very hard)'.PHP_EOL.
							PHP_EOL;
					} else {
						// Analyze opponents
						if (@$players[PLAYER_INDEX]['active']) {
							$player = $players[PLAYER_INDEX];
							for ($i = 0, $j = count($players); $i < $j; ++$i) {
								$focus = $players[$i];
								if ($i != PLAYER_INDEX && $focus['active']) {
									// Determine pursue action
									$pursue = '';
									$dx = $player['x'] - $focus['x'];
									$dy = $player['y'] - $focus['y'];
									$dz = $player['z'] - $focus['z'];
									// fb: front/back, lr: left/right
									$fb = $dx * $player['sin'] + $dy * $player['cos'];
									$lr = $dx * $player['cos'] - $dy * $player['sin'];
									// Determine right or left
									if ($fb > FB_CUTOFF) {
										if ($lr < -LR_CUTOFF)
											$pursue .= 'R';
										else if ($lr > LR_CUTOFF)
											$pursue .= 'L';
									} else if ($fb < -FB_CUTOFF) {
										if ($lr < 0)
											$pursue .= 'R';
										else if ($lr > 0)
											$pursue .= 'L';
										else
											$pursue .= ($focus['identifier'] & 1)? 'R': 'L';
									}
									// Determine ascend or descend
									if ($dz < -DZ_CUTOFF)
										$pursue .= 'A';
									else if ($dz > DZ_CUTOFF)
										$pursue .= 'D';
									// Determine probability and attack action
									$probability = 0;
									// Very close, highest probability
									if (abs($dx) < DX_CUTOFF && abs($dy) < DY_CUTOFF && abs($dz) < DZ_CUTOFF)
										$probability = PROB_HI;
									// Only consider if player is behind the focus
									else if ($fb < 0)
										$probability = min(max(round((K1 - $lr * $lr - $dx * $dx - $dy * $dy - DZ_BIAS * $dz * $dz) / K2), PROB_LO), PROB_HI);
									// Attack option, if applicable, is pursue and identifier
									$attack = (!$player['loading_cd'] && $probability > 0)? $pursue.$focus['identifier']: '';
									// Determine threat and evasion action
									// Switch roles
									$dx = -$dx;
									$dy = -$dy;
									$dz = -$dz;
									// fb: front/back, lr: left/right
									$fb = $dx * $focus['sin'] + $dy * $focus['cos'];
									$lr = $dx * $focus['cos'] - $dy * $focus['sin'];
									$threat = 0;
									// Very close, highest threat
									if (abs($dx) < DX_CUTOFF && abs($dy) < DY_CUTOFF && abs($dz) < DZ_CUTOFF)
										$threat = PROB_HI;
									// Only consider if the focus is behind the player
									else if ($fb < 0)
										$threat = min(max(round((K1 - $lr * $lr - $dx * $dx - $dy * $dy - DZ_BIAS * $dz * $dz) / K2), PROB_LO), PROB_HI);
									$evade = '';
									if ($threat > 0) {
										// Determine right or left
										if ($lr < 0)
											$evade .= 'R';
										else if ($lr > 0)
											$evade .= 'L';
										else
											$evade .= ($focus['identifier'] & 1)? 'R': 'L';
										// Determine ascend or descend action
										$atTop = strpos($events, 'reached top') > 0;
										$atBtm = strpos($events, 'reached bottom') > 0;
										if ($dz < 0)
											$evade .= ($atTop? 'D': 'A');
										else if ($dz > 0)
											$evade .= ($atBtm? 'A': 'D');
										else {
											if ($atTop)
												$evade .= 'D';
											else if ($atBtm)
												$evade .= 'A';
											else
												$evade .= ($focus['identifier'] & 1)? 'A': 'D';
										}
									}
									// Format
									$swot .= ' '.$focus['identifier'].'  '.
											' '.($focus['level'] + 1).'  '.
											substr(' '.$focus['health'], -2).'  '.
											substr($pursue.'  ', 0, 2).'   '.
											substr(' '.$probability, -2).'  '.
											substr($attack.'   ', 0, 3).'   '.
											substr(' '.$threat, -2).'  '.
											substr($evade.'  ', 0, 2).
											PHP_EOL;
								}
							}
							if ($swot) {
								// Randomize the example so as to not bias the remote AI
								shuffle($lrExtra);
								shuffle($adExtra);
								$swot = '# SWOT Analysis: Based on your strategy pick an action'.PHP_EOL.
									PHP_EOL.
									'Your level: '.($player['level'] + 1).' ('.
									'ship is '.$shipSpeed[$player['gear'] - 1].', '.
									'missiles deal '.$player['damage'].' damage, '.
									'and can change direction '.$shipDelta[$player['mss_delta'] - 1].' any ship)'.PHP_EOL.
									'Your health: '.$player['health'].' (out of 60)'.PHP_EOL.
									PHP_EOL.
									'Action codes:'.PHP_EOL.
									'L = turn left'.PHP_EOL.
									'R = turn right'.PHP_EOL.
									'A = ascend'.PHP_EOL.
									'D = descend'.PHP_EOL.
									'1,2,3 = fire missile at that player ID'.PHP_EOL.
									'X = fire missile at no particular target, e.g. for sensing environment'.PHP_EOL.
									'Actions may be combined (e.g., '.strtoupper($lrExtra[0][0].$adExtra[0][0]).' = turn '.$lrExtra[0].' and '.$adExtra[0].').'.PHP_EOL.
									PHP_EOL.
									'Column Legend'.PHP_EOL.
									'ID  : player ID'.PHP_EOL.
									'OL  : opponent level'.PHP_EOL.
									'OH  : opponent health'.PHP_EOL.
									'PA  : pursue action'.PHP_EOL.
									'ATK : attack probability'.PHP_EOL.
									'AA  : attack action'.PHP_EOL.
									'THR : threat probability'.PHP_EOL.
									'EA  : evasion action'.PHP_EOL.
									PHP_EOL.
									'Higher ATK means better chance to damage the opponent.'.PHP_EOL.
									'Higher THR means greater danger from that opponent.'.PHP_EOL.
									PHP_EOL.
									'Select the best action from PA, AA, or EA based on your strategy.'.PHP_EOL.
									'Return the action code exactly as written.'.PHP_EOL.
									PHP_EOL.
									'ID  OL  OH  PA  ATK  AA   THR  EA'.PHP_EOL.
									$swot.
									PHP_EOL;
							}
						} else if (!$waiting) {
							$actions .= '# Remote AI'.PHP_EOL.
								PHP_EOL.
								'No action required. Wait for the final scores.'.PHP_EOL.
								'You can update your internal notes if useful for your strategy.'.PHP_EOL.
								PHP_EOL;
							$waiting = true;
						}
					}
					if ($events) {
						if (@$players[PLAYER_INDEX]['npc'])
							$events = '# Relevant Events'.PHP_EOL.
								PHP_EOL.
								'When you join, you will be playing as player 0.'.PHP_EOL.
								PHP_EOL.
								$events.
								PHP_EOL;
						else
							$events = '# Relevant Events: Take notes as required'.PHP_EOL.
								PHP_EOL.
								'These events occurred recently.'.PHP_EOL.
								'You are playing as player 0.'.PHP_EOL.
								'You can update your internal notes if useful for your strategy.'.PHP_EOL.
								PHP_EOL.
								$events.
								PHP_EOL;
					}
					if ($scores) {
						$scores = '# Final Scores: Take notes as required'.PHP_EOL.
							PHP_EOL.
							'Your player ID is 0.'.PHP_EOL.
							PHP_EOL.
							'Review the outcome and update your internal notes if useful for your strategy.'.PHP_EOL.
							PHP_EOL.
							'Column Legend'.PHP_EOL.
							'ID : player ID'.PHP_EOL.
							'WI : winner? Y/N (last player remaining)'.PHP_EOL.
							'SC : score (the winner may not have the highest score)'.PHP_EOL.
							PHP_EOL.
							'Second place goes to the player with the highest score among the non-winners.'.PHP_EOL.
							'If there is a tie for second place, one is chosen effectively at random.'.PHP_EOL.
							'During training, the winner and second place go on to the next generation.'.PHP_EOL.
							PHP_EOL.							
							'ID  WI    SC'.PHP_EOL.
							$scores.
							PHP_EOL;
						file_put_contents(SCORES, json_encode($scores).PHP_EOL, FILE_APPEND);
						$waiting = false;
					}
					// What to send to PVP-AI
					$output = '';
					// Create information payload
					$payload = rtrim($events.$scores.$actions.$swot, "\r\n");
					if ($payload) {
					/*
						$encoded = file_get_contents('pvp-ai-'.$llm.'.json');
						$encoded = str_replace(
							array(
								'[-notes-]',
								'[-payload-]'
							),
							array(
								substr(json_encode($notes), 1, -1),
								substr(json_encode($payload), 1, -1)
							), $encoded);
					*/
						// LLM INTEGRATION OMITTED
						//
						// The full API call and response parsing logic are intentionally
						// excluded because they contain proprietary prompt formats and control
						// structures. Implementers can insert their own integrations here.

						// Replaced with dummy functionality: join as a priority target, then move & fire randomly
						if (@$players[PLAYER_INDEX]['npc'])
							$output = 'F';
						else if (@$players[PLAYER_INDEX]['active']) {
							$output = strtoupper($lrExtra[0][0].$adExtra[0][0]);
							if (!@$players[PLAYER_INDEX]['loading_cd'] && !(rand() & 3))
								$output .= 'X';
						}
					}
					// Forward the output to PVP-AI
					$output .= CHT_EOL;
					socket_write($client, $output, strlen($output));
					$players = array();
					$events = "";
					$scores = "";
					$ready = false;
				}
			}
		}
	}
	// Clean up
	@socket_close($client);
	@socket_close($server);
	@unlink($socket);
}
