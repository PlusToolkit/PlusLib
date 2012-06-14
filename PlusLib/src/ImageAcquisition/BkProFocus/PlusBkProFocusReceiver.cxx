/*


//////////// IAcquitisionDataReceiver interface
bool vtkBKOEMVideoSource::Prepare
{
	// If we don't have callback then don't try to receive data.
	if (!this->ValidCallback)
	{
		this->okToReceiveData = false;
	}
}


bool vtkBKOEMVideoSource::DataAvailable(int lintes, int pitch, void const* frameData)
{
	//// Code from bmodeWidget/bmodeViewDataReceiver
	this->mutexOkToRecvData.lock();
	if (! this->okToReceiveData)
	{
		//LeaveCriticalSection(&this->critOkToRecvData);
		this->mutexOkToRecvData.unlock();
		return true;
	}

	const ResearchInterfaceLineHeader* header = reinterpret_cast<const ResearchInterfaceLineHeader*>(frameData);

    if(this->frame == NULL || this->bmodeFrame == NULL)
    {
		//LeaveCriticalSection(&this->critOkToRecvData);
		this->mutexOkToRecvData.unlock();
        return false;
    }

    // decimate received data into frame
    int bytesPerSample = 2;

    const unsigned char* inputFrame = reinterpret_cast<const unsigned char*>(frameData);
    int numBmodeLines = 0;

    for(int i = 0; i < this->params.n_lines && numBmodeLines < this->MaxNumLines; ++i)
    {
        const int32_t* currentInputPosition = reinterpret_cast<const int32_t*>(inputFrame + i*pitch);
        header =  reinterpret_cast<const ResearchInterfaceLineHeader*>(currentInputPosition);


        if (header->ModelID == 0 && header->CFM == 0 && header->FFT ==0)
        {

            int32_t* currentOutputPosition = reinterpret_cast<int32_t*>(this->frame + numBmodeLines*this->params.n_samples*bytesPerSample);


            // n_samples is 16 bit samples, but we need to iterate over 32 bit iq samples, 
            // so divide by 2 to get the right number
            for(int j = 0; j < this->params.n_samples /this->decimation; ++j)
            {
                *currentOutputPosition = *currentInputPosition;
                currentInputPosition += this->decimation;
                currentOutputPosition += 1;
            }

            ++numBmodeLines;
        }
	}

    // compute bmode
    if(numBmodeLines > 0)
    {
        int tempLines = this->params.n_lines;
        this->params.n_lines = numBmodeLines;
        bmode_set_params_sqrt(&params);
        bmode_detect_compress_sqrt_16sc_8u(reinterpret_cast<int16_t*>(this->frame), this->bmodeFrame, &(this->params));
        this->params.n_lines = tempLines;
        bmode_set_params_sqrt(&params);
		
		this->DoCallback();
	}
	this->mutexOkToRecvData.unlock();
	return true;
}

void vtkBKOEMVideoSource::DoCallback()
{
	// int frameSize[2] = [numLines, numSamples]
	this->CallbackPointer(
}

void SetCallback(int *cb)
{
	this->CallbackPointer = cb;
}

*/