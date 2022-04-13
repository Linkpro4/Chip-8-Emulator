#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <time.h>

typedef struct Chip8State {
	uint8_t V[16];
	uint16_t stack[16];
	uint16_t I;
	uint16_t SP;
	uint16_t PC;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint8_t *memory;
	uint32_t video[64 * 32];
	uint8_t keypad[16];
} Chip8State;

typedef struct Platform {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
} Platform;

uint8_t font4x5[] = {
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8State* InitChip8(void)
{
	Chip8State* s = calloc(sizeof(Chip8State), 1);

	s->memory = calloc(1024 * 4, 1);
	s->PC = 0x200;

	memcpy(&s->memory[0x50], font4x5, 5 * 16);
	return s;
}

Platform* InitPlatform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
	Platform* p = calloc(sizeof(Platform), 1);

	SDL_Init(SDL_INIT_VIDEO);

	p->window = SDL_CreateWindow(title,100,100,windowWidth,windowHeight,SDL_WINDOW_SHOWN);
	p->renderer = SDL_CreateRenderer(p->window, -1, SDL_RENDERER_ACCELERATED);
	p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,textureWidth,textureHeight);

	return p;
}

int ProcessInput(struct Chip8State *state){
	int quit = 0;
	SDL_Event event;

	while (SDL_PollEvent(&event)){
		switch (event.type){
			case SDL_QUIT:
			{
				quit = 1;
			} break;

			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					{
						quit = 1;
					}
					break;
					case SDLK_x:
					{
						state->keypad[0] = 1;
					} break;

					case SDLK_1:
					{
						state->keypad[1] = 1;
					} break;

					case SDLK_2:
					{
						state->keypad[2] = 1;
					} break;

					case SDLK_3:
					{
						state->keypad[3] = 1;
					} break;

					case SDLK_q:
					{
						state->keypad[4] = 1;
					} break;

					case SDLK_w:
					{
						state->keypad[5] = 1;
					} break;

					case SDLK_e:
					{
						state->keypad[6] = 1;
					} break;

					case SDLK_a:
					{
						state->keypad[7] = 1;
					} break;

					case SDLK_s:
					{
						state->keypad[8] = 1;
					} break;

					case SDLK_d:
					{
						state->keypad[9] = 1;
					} break;

					case SDLK_z:
					{
						state->keypad[0xA] = 1;
					} break;

					case SDLK_c:
					{
						state->keypad[0xB] = 1;
					} break;

					case SDLK_4:
					{
						state->keypad[0xC] = 1;
					} break;

					case SDLK_r:
					{
						state->keypad[0xD] = 1;
					} break;

					case SDLK_f:
					{
						state->keypad[0xE] = 1;
					} break;

					case SDLK_v:
					{
						state->keypad[0xF] = 1;
					} break;
				}
			} break;

			case SDL_KEYUP:
			{
				switch (event.key.keysym.sym)
				{
					case SDLK_x:
					{
						state->keypad[0] = 0;
					} break;

					case SDLK_1:
					{
						state->keypad[1] = 0;
					} break;

					case SDLK_2:
					{
						state->keypad[2] = 0;
					} break;

					case SDLK_3:
					{
						state->keypad[3] = 0;
					} break;

					case SDLK_q:
					{
						state->keypad[4] = 0;
					} break;

					case SDLK_w:
					{
						state->keypad[5] = 0;
					} break;

					case SDLK_e:
					{
						state->keypad[6] = 0;
					} break;

					case SDLK_a:
					{
						state->keypad[7] = 0;
					} break;

					case SDLK_s:
					{
						state->keypad[8] = 0;
					} break;

					case SDLK_d:
					{
						state->keypad[9] = 0;
					} break;

					case SDLK_z:
					{
						state->keypad[0xA] = 0;
					} break;

					case SDLK_c:
					{
						state->keypad[0xB] = 0;
					} break;

					case SDLK_4:
					{
						state->keypad[0xC] = 0;
					} break;

					case SDLK_r:
					{
						state->keypad[0xD] = 0;
					} break;

					case SDLK_f:
					{
						state->keypad[0xE] = 0;
					} break;

					case SDLK_v:
					{
						state->keypad[0xF] = 0;
					} break;
				}
			} break;
		}
	}

	return quit;
}

void PrintState(struct Chip8State *state) {
	printf("PC - %04x | Memory - %02x%02x | V0 - %02x | V1 - %02x | V2 - %02x | V3 - %02x | V4 - %02x | V5 - %02x | V6 - %02x | V7 - %02x | SP - %04x | ST - %02x | DT - %02x | I - %02x |\n", 
	state->PC, state->memory[state->PC],state->memory[state->PC+1], state->V[0], state->V[1], state->V[2], state->V[3], state->V[4], state->V[5], state->V[6], 
	state->V[7], state->SP,state->soundTimer, state->delayTimer, state->I);
}

void Update(void const* buffer, int pitch, struct Platform *platform) {
	SDL_UpdateTexture(platform->texture, NULL, buffer, pitch);
	SDL_RenderClear(platform->renderer);
	SDL_RenderCopy(platform->renderer, platform->texture, NULL, NULL);
	SDL_RenderPresent(platform->renderer);
}


void EmulateChip8Op(Chip8State *state) {
	uint8_t *op = &state->memory[state->PC];
	uint8_t *code = op;
	int highnib = (*op & 0xf0) >> 4;
	uint16_t fullOpcode = (code[0] << 8) | code[1];
	uint8_t x = code[0] & 0x0F;
	uint8_t y = (code[1] & 0xF0) >> 4;
	uint16_t nnn = (code[0] & 0xF) << 8 | code[1];

	printf("OP - %04x   ", fullOpcode);

	state->PC += 2;

	//printf("%04x", fullOpcode);
	PrintState(state);

	switch (highnib)
	{
	case 0x00:
		switch (fullOpcode)
		{
		case 0x00E0:
		{
			memset(state->video, 0, sizeof(state->video));
		}
		break;
		case 0x00EE:
		{
			state->SP = state->SP - 1;
			state->PC = state->stack[state->SP];
		}
		break;
		}
		break;
	case 0x01:
	{
			state->PC = nnn;
	}
	break;
	case 0x02:
	{
		state->stack[state->SP] = state->PC;
		state->SP++;
		state->PC = nnn;
	}
	break;
	case 0x03:
	{
		if (state->V[x] == code[1]) {
			state->PC += 2;
		}
	}
	break;
	case 0x04:
	{
		if (state->V[x] != code[1]) {
			state->PC += 2;
		}
	}
	break;
	case 0x05:
	{
		if (state->V[x] == state->V[y]) {
			state->PC += 2;
		}
	}
	break;
	case 0x06:
	{
		state->V[x] = code[1];
	}
	break;
	case 0x07:
	{
		state->V[x] = state->V[x] + code[1];
	}
	break;
	case 0x08:
		switch (code[1] & 0x0F)
		{
		case 0x0:
		{
			state->V[x] = state->V[y];
		}
		break;
		case 0x1:
		{
			state->V[x] = state->V[x] | state->V[y];
		}
		break;
		case 0x2:
		{
			state->V[x] = state->V[x] & state->V[y];
		}
		break;
		case 0x3:
		{
			state->V[x] = state->V[x] ^ state->V[y];
		}
		break;
		case 0x4:
		{
			uint16_t answer = state->V[x] + state->V[y];
			state->V[x] = answer;
			if (answer > 0xFF) {
				state->V[0xF] = 1;
			}
			else {
				state->V[0xF] = 0;
			}
		}
		break;
		case 0x5:
		{
			if (state->V[x] > state->V[y]) {
				state->V[0xF] = 1;
			}
			else {
				state->V[0xF] = 0;
			}

			state->V[x] = state->V[x] - state->V[y];
		}
		break;
		case 0x6:
		{
			if (state->V[x] & 0x1) {
				state->V[0xF] = 1;
			}
			else {
				state->V[0xF] = 0;
			}

			state->V[x] = (state->V[x] >> 1);
		}
		break;
		case 0x7:
		{
			if (state->V[x] < state->V[y]) {
				state->V[0xF] = 1;
			}
			else {
				state->V[0xF] = 0;
			}

			state->V[x] = state->V[y] - state->V[x];
		}
		break;
		case 0xE:
		{
			state->V[0xF] = 1;

			if (state->V[x] & 0x80) {
				state->V[0xF] = 1;
			}
			else {
				state->V[0xF] = 0;
			}

			state->V[x] = (state->V[x] <<= 1);
		}
		break;
		}
		break;
	case 0x09:
	{
		if (state->V[x] != state->V[y]) {
			state->PC += 2;
		}
	}
	break;
	case 0x0a:
	{
		state->I = nnn;
	}
	break;
	case 0x0b:
	{
		state->PC = nnn + state->V[0x0];
	}
	break;
	case 0x0c:
	{
		time_t t;

		//Fix this because i know it can be done better
		srand((unsigned)time(&t));
		state->V[x] = (rand() * 255) & code[1];
		//printf("%02x", state->V[x]);
	}
	break;
	case 0x0d:
	{
		uint8_t height = fullOpcode & 0x000F;

		unsigned int video_width = 64;
		unsigned int video_height = 32;

		uint8_t xPos = state->V[x] % video_width;
		uint8_t yPos = state->V[y] % video_height;

		state->V[0xF] = 0;

		for (unsigned int row = 0; row < height; ++row) {
			uint8_t spriteByte = state->memory[state->I + row];

			for (unsigned int col = 0; col < 8; ++col) {
				uint8_t spritePixel = spriteByte & (0x80 >> col);
				uint32_t *screenPixel = &state->video[(yPos + row) * video_width + (xPos + col)];

				if (spritePixel) {
					if (*screenPixel == 0xFFFFFFFF) {
						state->V[0xF] = 1;
					}

					*screenPixel ^= 0xFFFFFFFF;
				}
			}
		}
	}
	break;
	case 0x0e:
	{
		switch (code[1])
		{
		case 0x9E:
		{
			uint8_t key = state->V[x];

			if (state->keypad[key]) {
				state->PC += 2;
			}
		}
		break;
		case 0xA1:
		{
			uint8_t key = state->V[x];

			if (!state->keypad[key]) {
				state->PC += 2;
			}
		}
		break;
		}
	}
	break;
	case 0x0f:
		switch (code[1])
		{
		case 0x07:
		{
			state->V[x] = state->delayTimer;
		}
		break;
		case 0x0A:
		{
			if (state->keypad[0]) {
				state->V[x] = 0;
			}
			else if (state->keypad[1]) {
				state->V[x] = 1;
			}
			else  if (state->keypad[2]) {
				state->V[x] = 2;
			}
			else  if (state->keypad[3]) {
				state->V[x] = 3;
			}
			else  if (state->keypad[4]) {
				state->V[x] = 4;
			}
			else  if (state->keypad[5]) {
				state->V[x] = 5;
			}
			else  if (state->keypad[6]) {
				state->V[x] = 6;
			}
			else  if (state->keypad[7]) {
				state->V[x] = 7;
			}
			else  if (state->keypad[8]) {
				state->V[x] = 8;
			}
			else  if (state->keypad[9]) {
				state->V[x] = 9;
			}
			else  if (state->keypad[10]) {
				state->V[x] = 10;
			}
			else  if (state->keypad[11]) {
				state->V[x] = 11;
			}
			else  if (state->keypad[12]) {
				state->V[x] = 12;
			}
			else  if (state->keypad[13]) {
				state->V[x] = 13;
			}
			else  if (state->keypad[14]) {
				state->V[x] = 14;
			}
			else  if (state->keypad[15]) {
				state->V[x] = 15;
			}
			else {
				state->PC -= 2;
			}
		}
		break;
		case 0x15:
		{
			state->delayTimer = state->V[x];
		}
		break;
		case 0x18:
		{
			state->soundTimer = state->V[x];
		}
		break;
		case 0x1E:
		{
			state->I += state->V[x];
		}
		break;
		case 0x29:
		{
			uint8_t digit = state->V[x];

			state->I = 0x50 + (5 * digit);
		}
		break;
		case 0x33:
		{
			uint8_t ones, tens, hundreds;
			uint8_t value = state->V[x];
			ones = value % 10;
			value = value / 10;
			tens = value % 10;
			hundreds = value / 10;
			state->memory[state->I] = hundreds;
			state->memory[state->I + 1] = tens;
			state->memory[state->I + 2] = ones;
		}
		break;
		case 0x55:
		{
			for (uint8_t i = 0; i <= x; i++) {
				state->memory[state->I + i] = state->V[i];
			}
		}
		break;
		case 0x65:
		{
			for (uint8_t i = 0; i <= x; i++) {
				state->V[i] = state->memory[state->I + i];
			}
		}
		break;
		default:
			PrintState(state);
			break;
		}
		break;
	}
}

void DisassembleChip8Op(uint8_t *codebuffer, int pc) {
	uint8_t *code = &codebuffer[pc];
	uint8_t firstnib = (code[0] >> 4);
	uint16_t fullcode = (code[0] << 8) | code[1];


	printf("%04x %02x %02x", pc, code[0], code[1]);
	switch (firstnib) {
	case 0x00:
		switch (fullcode) {
		case 0x00E0:
		{
			printf(" CLS");
		}
		break;
		case 0x00EE:
		{
			printf(" RTS");
		}
		break;
		}
		break;
	case 0x01:
	{
		uint8_t addressn = code[0] & 0x0f;
		printf("%-10s #$%01x%02x", " JUMP", addressn, code[1]);
	}
	break;
	case 0x02:
	{
		uint8_t addressn = code[0] & 0x0f;
		printf("%-10s #$%01x%02x", " CALL", addressn, code[1]);
	}
	break;
	case 0x03:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01x, #$%02x", " SKIP.EQ", reg, code[1]);
	}
	break;
	case 0x04:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01x, #$%02x", " SKIP.NE", reg, code[1]);
	}
	break;
	case 0x05:
	{
		uint8_t reg1 = code[0] & 0x0f;
		uint8_t reg2 = code[1] >> 4;
		printf("%-10s V%01x, V%01x", " SKIP.EQ", reg1, reg2);
	}
	break;
	case 0x06:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01x, #$%02x", " MVI", reg, code[1]);
	}
	break;
	case 0x07:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01x, #$%02x", " ADD", reg, code[1]);
	}
	break;
	case 0x08:
		switch (fullcode & 0xF)
		{
		case 0x0:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " MOV", reg1, reg2);
		}
		break;
		case 0x1:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " OR", reg1, reg2);
		}
		break;
		case 0x2:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " AND", reg1, reg2);
		}
		break;
		case 0x3:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " XOR", reg1, reg2);
		}
		break;
		case 0x4:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " ADD.", reg1, reg2);
		}
		break;
		case 0x5:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " SUB.", reg1, reg2);
		}
		break;
		case 0x6:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x", " SHR.", reg1);
		}
		break;
		case 0x7:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x, V%01x", " SUBB.", reg1, reg2);
		}
		break;
		case 0xE:
		{
			uint8_t reg1 = code[0] & 0x0f;
			uint8_t reg2 = code[1] >> 4;
			printf("%-10s V%01x", " SHL.", reg1);
		}
		break;
		}
		break;
	case 0x09:
	{
		uint8_t reg1 = code[0] & 0x0f;
		uint8_t reg2 = code[1] >> 4;
		printf("%-10s V%01x, V%01x", " SKIP.NE", reg1, reg2);
	}
	break;
	case 0x0a:
	{
		uint8_t addresshi = code[0] & 0x0f;
		printf("%-10s I,#$%01x%02x", " MVI", addresshi, code[1]);
	}
	break;
	case 0x0b:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V0, #$%01x%02x", " JUMP", reg, code[1]);
	}
	break;
	case 0x0c:
	{
		uint8_t reg = code[0] & 0x0f;
		printf("%-10s V%01x, #$%02x", " RND", reg, code[1]);
	}
	break;
	case 0x0d:
	{
		uint8_t reg1 = code[0] & 0x0f;
		uint8_t reg2 = code[1] >> 4;
		uint8_t lastNibble = code[1] & 0xF;
		printf("%-10s V%01x, V%01x, #$%01x", " SPRITE", reg1, reg2, lastNibble);
	}
	break;
	case 0x0e:
		switch (fullcode & 0xFF)
		{
		case 0x9E:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x", " SKIP.KEY", reg);
		}
		break;
		case 0xA1:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x", " SKIP.NOKEY", reg);
		}
		break;
		}
		break;
	case 0x0f:
		switch (fullcode & 0xFF)
		{
		case 0x07:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x, DELAY", " MOV", reg);
		}
		break;
		case 0x0A:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x", " WAITKEY", reg);
		}
		break;
		case 0x15:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s DELAY, V%01x", " MOV", reg);
		}
		break;
		case 0x18:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s SOUND, V%01x", " MOV", reg);
		}
		break;
		case 0x1E:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s I, V%01x", " ADD", reg);
		}
		break;
		case 0x29:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x", " SPRITECHAR", reg);
		}
		break;
		case 0x33:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V%01x", " MOVBCD", reg);
		}
		break;
		case 0x55:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s (I), V0-V%01x", " MOVM", reg);
		}
		break;
		case 0x65:
		{
			uint8_t reg = code[0] & 0x0f;
			printf("%-10s V0-V%01x, (I)", " MOVM", reg);
		}
		break;
		}
		break;
	}
}


void cycle(struct Chip8State *state) {
	EmulateChip8Op(state);

	if (state->delayTimer > 0) {
		--state->delayTimer;
	}

	if (state->soundTimer > 0) {
		--state->soundTimer;
	}
}

void ReadRomIntoMemory(char**argv, struct Chip8State *state){
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("error: Couldn't open%s\n", argv[1]);
		exit(1);
	}

	//Get the file size
	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	//Read Rom into memory
	fread(&state->memory[0x200], fsize, 1, f);
	fclose(f);
}


int main(int argc, char**argv) {
	//printf("Hello World");

	if (argc < 4){
		printf("Not enough cash... stranger");
		exit(1);
	}
	//Initialize CPU
	struct Chip8State *state = InitChip8();
	//Arguments (ROM) (VIDEOSCALE) (CYCLE DELAY)
	int videoScale =  atoi(argv[2]);
	int cycleDelay =  atoi(argv[3]);

	//Initialize Video
	struct Platform *platform = InitPlatform("Chip-8 Emulator",64*videoScale,32*videoScale,64,32);
	ReadRomIntoMemory(argv, state);

	int videoPitch = sizeof(state->video[0]) * 64;


	float lastCycleTime = clock();
	int quit = 0;

	while (!quit) {
		//DisassembleChip8Op(buffer,pc);

		quit = ProcessInput(state);

		float currentTime = clock();
		float dt = currentTime - lastCycleTime;

		if (dt > cycleDelay){

			lastCycleTime = currentTime;

			cycle(state);

			Update(state->video, videoPitch, platform);
		}
		//printf("\n");
	}
	return 0;
}