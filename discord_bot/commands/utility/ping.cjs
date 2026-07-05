//import { SlashCommandBuilder } from 'discord.js';
const { SlashCommandBuilder } = require('discord.js');

// Object: {data, response}
module.exports = {
	data: new SlashCommandBuilder().setName("ping").setDescription("Replies with pong!"),
	async execute(interaction) {
		await interaction.reply('Pong!');
	},
};
