import { REST, Routes } from 'discord.js';
import * as fs from 'fs';
import * as path from 'path';
import 'dotenv/config';
import { pathToFileURL } from 'url';

const commands = [];

const __dirname = process.cwd();
const folderPath = path.join(__dirname, 'commands');
const commandFolders = fs.readdirSync(folderPath);

for (const folder of commandFolders) {

	const commandsPath = path.join(folderPath, folder);
	const commandFiles = fs.readdirSync(commandsPath).filter(file => file.endsWith('.cjs'));

//	console.log("Command Files: ", commandFiles);

	for (const file of commandFiles) {
		const filePath = path.join(commandsPath, file);
//		console.log("File Path: ", filePath);
		
		const reqPath = pathToFileURL(path.relative(__dirname, filePath));
		const command = await import(reqPath.href);

//		console.log("File Path: ", reqPath);
//		console.log("Command: ", command);
		if (command.data && command.default.execute) {
			commands.push(command.data.toJSON());
			console.log("Adding command!\n");
		} else {
			console.error(`The command at ${filePath} is missing "Data" or "Execute" properties.`);
		}
	}
}

// Constructing a REST instance
const rest = new REST().setToken(process.env.DISCORD_TOKEN);

(async () => {
	try {
		console.log(`Started refreshing ${commands.length} application (/) commands.`);
		
		// put method to update commands in the guild
		const data = await rest.put(Routes.applicationGuildCommands(process.env.CLIENT_ID, process.env.GUILD_ID), {body: commands} );

		console.log(`Successfully reloaded ${commands.length} application (/) commands.`);
	}
	catch (error) {
		console.error(error);
	}
})();
