#include "ManagerRender.hpp"

#include <Scenes/Scenes.hpp>
#include <Renderer/Renderer.hpp>
#include <Worlds/Worlds.hpp>

namespace Demo
{
	RenderpassCreate *RENDERPASS_CREATE = new RenderpassCreate
		{
			{
				// 0, depth
				// 1, swapchain
				ImageAttachment(2, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {{0.0f, 0.0f, 0.0f, 0.0f}}), // colours
				ImageAttachment(3, VK_FORMAT_R16G16_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {{0.0f, 0.0f, 0.0f, 0.0f}}), // normals
				ImageAttachment(4, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {{0.0f, 0.0f, 0.0f, 0.0f}}), // materials
				ImageAttachment(5, VK_FORMAT_R16_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, {{0.0f}}) // shadows
			}, // images
			{
				SubpassType(0, false, {5}),
				SubpassType(1, true, {2, 3, 4}),
				SubpassType(2, false, {1}),
				SubpassType(3, false, {1}),
			}, // subpasses
		};

	ManagerRender::ManagerRender() :
		IManagerRender(RENDERPASS_CREATE),
		m_infinity(Vector4(0.0f, 1.0f, 0.0f, +INFINITY)),
		m_rendererShadows(new RendererShadows(0)),
		m_rendererSkyboxes(new RendererSkyboxes(1)),
		m_rendererTerrains(new RendererTerrains(1)),
		m_rendererWaters(new RendererWaters(1)),
		m_rendererEntities(new RendererEntities(1)),
		m_rendererDeferred(new RendererDeferred(2)),
		m_filterFxaa(new FilterFxaa(3)),
		m_filterLensflare(new FilterLensflare(3)),
		m_filterTiltshift(new FilterTiltshift(3)),
		m_filterGrain(new FilterGrain(3)),
		m_rendererGuis(new RendererGuis(3)),
		m_rendererFonts(new RendererFonts(3))
	{
	}

	ManagerRender::~ManagerRender()
	{
		delete m_rendererShadows;

		delete m_rendererSkyboxes;
		delete m_rendererTerrains;
		delete m_rendererWaters;
		delete m_rendererEntities;

		delete m_rendererDeferred;
		delete m_filterFxaa;
		delete m_filterLensflare;
		delete m_filterTiltshift;
		delete m_filterGrain;
		delete m_rendererGuis;
		delete m_rendererFonts;
	}

	void ManagerRender::Render()
	{
		const auto commandBuffer = Renderer::Get()->GetCommandBuffer();
		const auto camera = Scenes::Get()->GetCamera();

		// Starts Rendering.
		VkResult startResult = Renderer::Get()->StartRenderpass(commandBuffer);

		if (startResult != VK_SUCCESS)
		{
			return;
		}

		// Subpass 0.
		m_rendererShadows->Render(commandBuffer, m_infinity, *camera);
		Renderer::Get()->NextSubpass(commandBuffer);

		// Subpass 1.
		m_rendererSkyboxes->Render(commandBuffer, m_infinity, *camera);
		m_rendererTerrains->Render(commandBuffer, m_infinity, *camera);
		m_rendererWaters->Render(commandBuffer, m_infinity, *camera);
		m_rendererEntities->Render(commandBuffer, m_infinity, *camera);
		Renderer::Get()->NextSubpass(commandBuffer);

		// Subpass 2.
		m_rendererDeferred->Render(commandBuffer, m_infinity, *camera);
		Renderer::Get()->NextSubpass(commandBuffer);

		// Subpass 3.
		m_filterFxaa->RenderFilter(commandBuffer);
		m_filterLensflare->SetSunPosition(*Worlds::Get()->GetSunPosition());
		m_filterLensflare->SetSunHeight(Worlds::Get()->GetSunHeight());
		m_filterLensflare->RenderFilter(commandBuffer);
		//m_filterTiltshift->RenderFilter(commandBuffer);
		//m_filterGrain->RenderFilter(commandBuffer);
		m_rendererGuis->Render(commandBuffer, m_infinity, *camera);
		m_rendererFonts->Render(commandBuffer, m_infinity, *camera);

		// Ends Rendering.
		Renderer::Get()->EndRenderpass(commandBuffer);
	}
}