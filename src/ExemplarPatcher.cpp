#include "ExemplarPatcher.h"
#include "Logger.h"
#include <GZServPtrs.h>
#include "cIGZPersistResourceManager.h"
#include "cIGZPersistResourceKeyList.h"
#include "cGZPersistResourceKey.h"
#include "cISCResExemplar.h"
#include "cISCResExemplarCohort.h"
#include "cISCPropertyHolder.h"
#include "cISCProperty.h"
#include "cIGZVariant.h"
#include "PersistResourceKeyFilterByTypeAndGroup.h"
/* #include <iostream> */


static constexpr uint32_t exemplarTid = 0x6534284a;
static constexpr uint32_t cohortTid = 0x05342861;
static constexpr uint32_t exemplarPatchGid = 0xb03697d1;  // GID of Cohort files
static constexpr uint32_t exemplarPatchTargetProperty = 0x0062e78a;
static constexpr uint32_t exemplarNameProperty = 0x20;


// Initializes the patches from available Cohort files. This function is called at PostAppInit.
void ExemplarPatcher::LoadPatches()
{
	cIGZPersistResourceManagerPtr pResMan;
	if (pResMan)
	{
		cIGZPersistResourceKeyList* pResourceList = nullptr;
		PersistResourceKeyFilterByTypeAndGroup* filter = new PersistResourceKeyFilterByTypeAndGroup(cohortTid, exemplarPatchGid);
		filter->AddRef();
		uint32_t res = pResMan->GetAvailableResourceList(&pResourceList, filter);
		filter->Release();

		if (pResourceList->Size() > 0)
		{
			pResourceList->EnumKeys([](const auto& key, void* pContext) {
				auto this1 = static_cast<ExemplarPatcher*>(pContext);
				cIGZPersistResourceManagerPtr pResMan;
				cISCResExemplarCohort* cohort = nullptr;
				pResMan->GetResource(key, GZIID_cISCResExemplarCohort, reinterpret_cast<void**>(&cohort), 0, nullptr);
				auto propHolder = cohort->AsISCPropertyHolder();
				propHolder->AddRef();

				bool isPatch = propHolder->HasProperty(exemplarPatchTargetProperty);

				if (isPatch)
				{
					auto property = propHolder->GetProperty(exemplarPatchTargetProperty);
					property->AddRef();
					const auto variant = property->GetPropertyValue();
					variant->AddRef();
					uint32_t reps = variant->GetCount();

					if (variant->GetType() != cIGZVariant::Type::Uint32Array)
					{
						Logger& logger = Logger::GetInstance();
						logger.WriteLineFormatted(LogLevel::Error,
								"Exemplar Patch Target property requires type Uint32Array: T:0x%08X G:0x%08X I:0x%08X.",
								key.type, key.group, key.instance);
					}
					else if (reps % 2 != 0)
					{
						Logger& logger = Logger::GetInstance();
						logger.WriteLineFormatted(LogLevel::Error,
								"Exemplar Patch Target property requires even number of values: T:0x%08X G:0x%08X I:0x%08X.",
								key.type, key.group, key.instance);
					}
					else if (reps > 0)
					{
						uint32_t* values = variant->RefUint32();
						for (uint32_t i = 1; i < reps; i += 2)
						{
							auto targetTgi = cGZPersistResourceKey(exemplarTid, values[i-1], values[i]);
							if (this1->patches.contains(targetTgi))
							{
								propHolder->AddRef();
								this1->patches[targetTgi]->push_back(propHolder);
							}
							else
							{
								propHolder->AddRef();
								auto queue = new std::deque<cISCPropertyHolder*>{propHolder};
								this1->patches[targetTgi] = queue;
							}
						}
					}

					variant->Release();
					property->Release();
				}

				propHolder->Release();
				cohort->Release();
			}, this);
		}

		Logger& logger = Logger::GetInstance();
		logger.WriteLineFormatted(LogLevel::Info,
				"Loaded %d Exemplar patches targeting, in total, %d Exemplar files.", pResourceList->Size(), patches.size());

		pResourceList->Release();
	}
}


ExemplarPatcher::ExemplarPatcher() : refCount(0), patches({})
{
}

bool ExemplarPatcher::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIExemplarLoadHookTarget)
	{
		*ppvObj = static_cast<cIExemplarLoadHookTarget*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	*ppvObj = nullptr;
	return false;
}

uint32_t ExemplarPatcher::AddRef()
{
	return ++refCount;
}

uint32_t ExemplarPatcher::Release()
{
	if (refCount == 1)
	{
		for (auto&& p : patches) {
			for (auto&& propHolder : *(p.second)) {
				propHolder->Release();
			}
			delete p.second;
		}
		delete this;
		return 0;
	}
	else
	{
		return --refCount;
	}
}

void applyPatch(cISCPropertyHolder* target, cISCPropertyHolder* patch)
{
	patch->EnumProperties([](auto property, void* pContext) {
			auto&& target = static_cast<cISCPropertyHolder*>(pContext);
			uint32_t id = property->GetPropertyID();
			if (id != exemplarPatchTargetProperty && id != exemplarNameProperty) {
				target->AddProperty(property, /*bSendMsg*/ false);  // `true` results in crash
			}
		}, target);
}

void ExemplarPatcher::ExemplarLoaded(const char* const originalFunctionName, const cGZPersistResourceKey& key, cISCResExemplar* resExemplar)
{
	if (patches.contains(key)) {
		/* std::cout << "--> Applying Exemplar patches to 0x" << std::hex << key.type << " 0x" << key.group << " 0x" << key.instance << "\n"; */
		auto target = resExemplar->AsISCPropertyHolder();
		for (auto&& patch : *(patches[key])) {
			applyPatch(target, patch);
		}
	}
}
