# Compiling
`gcc`

# Running





# Game Rules
BASIC COMMANDS
attack(target)
move()
capture(target)


carrier: largest mobile ship
drones: controlled by parent, carry missiles
satellite: static object, for jamming or relaying command
missile: explosive and pulse, distributed by carrier
turret: persistent beam, large
relay: extend range of drones
shield: temporary protection, prevents outgoing

CARRIER
shield: 0/1
turret: 2/2
missile: 4/40
drone: 2/8
crew: 2/6
pod: 0/2

DRONE
missile: 4/4

SATELLITE
relay: 0/1
jammer: 0/1

COLONY
crew: 0/4
turret: 0/4
refractor: 0/1
gravform: 0/1
drone: 4/8
pod: 0/2




1. AI
	- update command queue
		conditions
	- attacks
		missile
		turret
	- launch
	- movement
		relational:
		orbit
		escape
		approach
		attack
		dock
		evade
		follow

		objective:
		stop
		continue
		go
		turn

2. CREATE
	launch
		drone
		missile
		%satellite
		%mamba
		escape pod
	%shield
	arrive
		carrier
		%mamba
		%colony

3. MOVE
	missile
	drone
	%mamba
	carrier
	turret

4. RESOLVE
	missile hits
	collisions
	weapons
	%boarding
	note keeping for clean-up

5. CLEAN


behaviors:
agent modes
	patrol = move.there + attack.if

agent-object
	approach = move.there
	%bring aboard
	
agent-obstacle
	avoid = move.around

agent-enemy
	attack
	%retreat
	%disable
	%board

parent-child
	guard = parent + move.with + attack.if
	launch
	return
	resupply, needs counter

agent-boundary
	turn away
