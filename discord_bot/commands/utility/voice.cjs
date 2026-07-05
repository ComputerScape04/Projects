const { SlashCommandBuilder, ChannelType } = require('discord.js');
const { joinVoiceChannel, GuildChannelManager, createAudioPlayer, createAudioResource, entersState, VoiceConnectionStatus } = require('@discordjs/voice');
require('dotenv').config();
const { path } = require('path');

const __path = "D:/Fun Projects/discord_bot/assets/audio.ogg";

module.exports = {	
	data: new SlashCommandBuilder().setName('join').setDescription('Joins the voice channel'),
	async execute(interaction) {
		const member = interaction.member;
	
		const channel = member.guild.channels.cache.get(interaction.channelId);

		if (channel.type !== ChannelType.GuildVoice) {
			return interaction.reply("You can call /join only within a voice channel.");
		}

		// create the voice connection
		const voiceConnection = joinVoiceChannel({
			channelId: channel.id,
			guildId: interaction.guild.id,
			adapterCreator: interaction.guild.voiceAdapterCreator,
			selfDeaf: false,
		});

		// create an audio player & audio resource
		const player = createAudioPlayer();
		const resource = createAudioResource(__path);

		voiceConnection.subscribe(player);	
		player.play(resource);

		voiceConnection.on(VoiceConnectionStatus.Ready, () => {
			console.log('The connection has entered the Ready state - ready to play audio!');
		});
	
		player.on('stateChange', (oldState, newState) => {
			console.log(`Status: ${oldState.status} -> ${newState.status}`);
		});

		player.on('error', (error) => {
			console.error("Error: " + error);
		});

		interaction.reply(`Successfully created a voice connection!\n`);

	}
};	
