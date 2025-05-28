#include "XAudio.h"

XAudio::~XAudio() { 
	xAudio2_.Reset();
	SoundUnLoad(&soundData_);
}

void XAudio::Initialize() {
	HRESULT result;

	IXAudio2MasteringVoice* masterVoice;

	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice);
}

void XAudio::SoundsAllLoad(){ 
	sound_ = SoundLoadWave("resources/Title.wav");
}

SoundData XAudio::SoundLoadWave(const char* filename) {
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());

	// RIFFヘッダの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダの確認
	file.read(reinterpret_cast<char*>(&format.chunk), sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= format.fmt.size());
	file.read(reinterpret_cast<char*>(format.fmt.data()), format.chunk.size);

	soundData_.wfex.Format.cbSize = static_cast<WORD>((format.chunk.size > sizeof(WAVEFORMATEX)) ? format.chunk.size - sizeof(WAVEFORMATEX) : 0);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// waveファイルを閉じる
	file.close();

	//  後でsoundDataにコピー
	memset(&soundData_.wfex, 0, sizeof(WAVEFORMATEXTENSIBLE)); // 初期化
	memcpy(&soundData_.wfex, format.fmt.data(), format.chunk.size);
	soundData_.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData_.bufferSize = data.size;

	return soundData_;
}

void XAudio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {
	HRESULT result;

	auto& fmt = soundData_.wfex.Format;

	// 波形フォーマットを基にSoundVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex.Format);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 音声データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void XAudio::SoundUnLoad(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}
