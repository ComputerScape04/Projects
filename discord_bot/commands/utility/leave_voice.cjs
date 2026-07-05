const { SlashCommandBuilder } = require('discord.js');
const { getVoiceConnection } = require('@discordjs/voice');
	
module.exports = {
	data: new SlashCommandBuilder().setName('leave').setDescription('Leaving a voice call'),
	async execute(interaction) {
		const connection = getVoiceConnection(interaction.guild.id);

		if (!connection)
			return interaction.reply("I'm not in voice channel");

		connection.destroy();

		interaction.reply("Disconnected from voice!");
	},
};
