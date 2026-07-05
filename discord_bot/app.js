import * as fs from 'fs';
import * as path from 'path';
import {Client, Events, GatewayIntentBits, Collection, MessageFlags} from 'discord.js';
import 'dotenv/config';
import { pathToFileURL } from 'url';
import { VoiceConnectionStatus } from '@discordjs/voice';

const __dirname = process.cwd();

// Client connecting to the bot
const client = new Client({ intents: [GatewayIntentBits.Guilds, GatewayIntentBits.GuildVoiceStates]});

client.commands = new Collection();

// login
client.login(process.env.DISCORD_TOKEN);

// we run this only once
client.once(Events.clientReady, readyClient => {
	console.log(`Logged in as ${readyClient.user.tag}!`);
});

//____ Reading the commands from the command folder _____________
const foldersPath = path.join(__dirname, 'commands');
const commandFolders = fs.readdirSync(foldersPath); // reading the contents of the folders in foldersPath

// loop through the command folders
for (const folder of commandFolders) {
	
	// check for js files 
	const commands = path.join(foldersPath, folder);
	const commandsFiles = fs.readdirSync(commands).filter(file => !file.endsWith('.js'));

	// for each js file, extract the name and execute methods
	for (const file of commandsFiles) {
		
		const filePath = path.join(commands, file);
		;
		const req_file = pathToFileURL(path.relative(__dirname, filePath));
		
		//console.log(req_file.href);
		const module = await import(req_file.href);
		const command = module;

		// key: name; value: command module
		if (command.data && command.default.execute) {
			client.commands.set(command.data.name, command);
		} else {
			console.log(`The command at ${filePath} is either missing a "Data" and/or "Execute" properties`);
		}
	}
}
//_______________________________________________________________

// _______ On receiving an interaction, handle it _______________
client.on(Events.InteractionCreate, async (interaction) => {
	if (!interaction.isChatInputCommand()) return;

	const command = interaction.client.commands.get(interaction.commandName);
	
	if (!command) {
		console.error(`Invalid command ${interaction.commandName}.`);
		return;
	}

	// reply
	try {
//		console.log("Interaction Object: ", interaction);

		// if the cmd is "join" and the user is not in a voice channel, then 
		if (interaction.commandName.toLocaleLowerCase() === 'join') {
			if (!interaction.member.voice.channel) 
				return interaction.reply("You need to be in a voice channel before calling /join");
		}

		await command.default.execute(interaction);
	}
	catch (error) {
		console.error(error);
		
		if (interaction.replied || interaction.deferred) {
			await interaction.followUp({
				content: 'There was an error while executing this command.',
				flags: MessageFlags.Ephemeral,
			});
		} else {
			await interaction.followUp({
				content: 'There was an error while executing this command.',
				flags: MessageFlags.Ephemeral,
			});
		}
	}
});
//________________________________________________________________

// error detection
process.on('unhandledRejection', (error) => {
	console.error(`Unhandled Exception Error: ${error}`);
});
