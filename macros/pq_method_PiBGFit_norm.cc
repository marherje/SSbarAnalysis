#include <iostream>
#include <vector>

using std::cout; using std::endl;
using std::vector;

Float_t bins_cos_fine[] = {-1.0,-0.98,-0.96,-0.94,-0.92,-0.90,-0.88,-0.86,-0.84,-0.82,-0.80,-0.75,-0.70,-0.60,-0.50,-0.40,-0.30,-0.20,-0.10,
                            0.0,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.75,0.80,0.82,0.84,0.86,0.88,0.90,0.92,0.94,0.96,0.98,1.0};
// Int_t   nbins_cos = sizeof(bins_cos_fine) / sizeof(Float_t) - 1;
Int_t   nbins_cos = 100;

Float_t bins_cos_fine_half[] = {0.0,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.75,0.80,0.82,0.84,0.86,0.88,0.90,0.92,0.94,0.96,0.98,1.0};
Int_t   nbins_cos_half = sizeof(bins_cos_fine_half) / sizeof(Float_t) - 1;

void BinNormal(TH1F *h)
{
  const Int_t nbins = h->GetNbinsX();
  for (int ibin=1; ibin<=nbins; ibin++){
    Float_t binc = h->GetBinContent(ibin);
    Float_t binw = h->GetBinWidth(ibin);
    Float_t bin_normal = binc / binw;
    h->SetBinContent(ibin,bin_normal);
  }

}

void Normalize(TH1F *h, Float_t norm_top)
{
  // h->Scale( 1.0 / h->GetEntries() );
  const Int_t nbins = h->GetNbinsX();
  Int_t nbins4 = nbins / 4;
  // Int_t int_high = (nbins / 2) + nbins4;
  // Int_t int_low  = (nbins / 2 + 1) - nbins4;
  // Int_t int_high = nbins-4;
  // Int_t int_low  = (nbins / 2);
  Int_t int_high = nbins-4;
  Int_t int_low  = (nbins / 2) + 4;
  h->Scale( norm_top / h->Integral(int_low,int_high) );
}

void NormalizeUU(TH1F *h, Float_t norm_top)
{
  const Int_t nbins = h->GetNbinsX();
  Int_t nbins4 = nbins / 4;
  Int_t int_high = nbins;
  Int_t int_low  = 1;

  cout << "uu integral = " << h->Integral(int_low,int_high) << endl;
  cout << "uu entries  = " << h->GetEntries() << endl;

  h->Scale( norm_top / h->Integral(int_low,int_high) );

}

void NormalizeGen(TH1F *h, Float_t norm_top)
{
  h->Scale( norm_top / h->GetEntries() );
}

void Normalize2Reco(TH1F *h_reco, TH1F *h_gen)
{
	double intCosReco = h_reco->Integral(10,90);
	double intCosGen  = h_gen->Integral(10,90);
  h_gen->Scale( intCosReco / intCosGen );
}

void StyleHist(TH1F *h, Color_t col)
{
  h->SetLineWidth(3);
  h->SetLineColor(col);
  h->SetFillStyle(3002);
  h->SetFillColor(col);
}

void StyleFunc(TF1 *f, Int_t styl, Color_t col)
{
  f->SetLineColor(col);
  f->SetLineWidth(5);
  f->SetLineStyle(styl);
}

void StylePad(TPad *pad, Float_t t, Float_t b, Float_t r, Float_t l)
{
  pad->SetGrid(1,1);
  if(t) pad->SetTopMargin(t);
  if(b) pad->SetBottomMargin(b);
  if(r) pad->SetRightMargin(r);
  if(l) pad->SetLeftMargin(l);
  pad->Draw();
  pad->cd();

}

vector<Float_t> GetP( TH1F * h_accepted, TH1F * h_rejected )
{
  const Int_t nbins = h_accepted->GetNbinsX();
  vector<Float_t> result_error;
  vector<Float_t> result;

  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    std::vector<float> result_j;
    for (int i1 = -1; i1 < 2; i1 += 2)
    {
      for (int i2 = -1; i2 < 2; i2 += 2)
      {
        for (int i3 = -1; i3 < 2; i3 += 2)
        {
          float accepted = h_accepted->GetBinContent(nbins + 1 - i) + i1 * sqrt(h_accepted->GetBinContent(nbins + 1 - i));
          float rejected = h_rejected->GetBinContent(nbins + 1 - i) + i2 * sqrt(h_rejected->GetBinContent(nbins + 1 - i));
          accepted += h_accepted->GetBinContent(i) + i3 * sqrt(h_accepted->GetBinContent(i));

          float a = 1;
          float b = -1;
          float c = rejected / (2 * (accepted + rejected));
          float p = (0.5 / a) * (-b + sqrt(b * b - 4 * a * c));
          float p2 = (0.5 / a) * (-b - sqrt(b * b - 4 * a * c));
          if (p > 0.99)
            p = 0;
          if (p2 > 0.99)
            p2 = 0;
          if (p > 0 || p2 > 0)
            result_j.push_back(max(p, p2));
        }
      }
    }

    float average = 0;
    float n = 0;
    for (unsigned j = 0; j < result_j.size(); j++)
    {
      if (result_j.at(j) > 0)
      {
        average += result_j.at(j);
        n++;
      }
    }
    average /= n;

    if ( (average != 0) && (average == average) )
    {
      result.push_back(average);
      float std_dev = 0;
      for (unsigned j = 0; j < result_j.size(); j++)
      {
        if (result_j.at(j) > 0)
        {
          std_dev += pow(result_j.at(j) - average, 2);
        }
      }
      std_dev = sqrt(std_dev / (n - 1));
      result_error.push_back(std_dev);
    }
    else
    {
      result_error.push_back(0);
      result.push_back(0);
    }
  }

  for (unsigned i = 0; i < result_error.size(); i++)
  {
    if (result_error.at(i) > 0)
      result.push_back(result_error.at(i));
    else
      result.push_back(0);
  }

  return result;

}

TH1F * CorrectHist( TH1F * h_reco, vector<Float_t> p_vec)
{
  const Int_t nbins = h_reco->GetNbinsX();

  TH1F *corrected = new TH1F("corrected", "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    float p = p_vec.at(i - 1);
    // float p = 0.9;
    float q = 1 - p;
    float weight = (p * p + q * q) / (q * q * q * q - p * p * p * p);

    // calcualte average
    float av_i = 0;
    float av_41i = 0;
    float n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        av_i += (np_reco_error * q * q - nm_reco_error * p * p) * weight;
        av_41i += -(np_reco_error * p * p - nm_reco_error * q * q) * weight;
        n++;
      }
    }
    av_i /= n;
    av_41i /= n;
    corrected->SetBinContent(i, av_i);
    corrected->SetBinContent(nbins + 1 - i, av_41i);

    // calculate error
    float error_i = 0;
    float error_41i = 0;
    n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        error_i += pow((np_reco_error * q * q - nm_reco_error * p * p) * weight - av_i, 2);
        error_41i += pow(-(np_reco_error * p * p - nm_reco_error * q * q) * weight - av_41i, 2);
        n++;
      }
    }
    error_i = sqrt(error_i / (n - 1.));
    error_41i = sqrt(error_41i / (n - 1.));
    corrected->SetBinError(i, error_i);
    corrected->SetBinError(nbins + 1 - i, error_41i);
  }

  return corrected;

}

TH1F * Efficiency_Correction( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_N_Pi_cos  = (TH1F*) file->Get("h_gen_N_Pi_cos");
  TH1F *h_reco_N_Pi_cos = (TH1F*) file->Get("h_reco_N_Pi_cos");
  TH1F *h_N_Pi_corr_cos = (TH1F*) file->Get("h_N_Pi_corr_cos");

  TH1F *h_stable_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  h_stable_cos->Divide(h_gen_N_Pi_cos);

  if( h->GetNbinsX() != h_stable_cos->GetNbinsX() ) throw std::logic_error("Error");

  Int_t nbins = h_stable_cos->GetNbinsX();
  TH1F *corrected = new TH1F(name.Data(), "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int ibin = 1; ibin < nbins + 1; ibin++){

    Float_t binc_h   = h->GetBinContent(ibin);
    Float_t binc_eff = h_stable_cos->GetBinContent(ibin);

    corrected->SetBinContent(ibin,binc_h / binc_eff);

  }

  return corrected;

}

TH1F * Efficiency_Correction2( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_N_Pi_cos  = (TH1F*) file->Get("h_gen_N_Pi_cos");
  TH1F *h_reco_N_Pi_cos = (TH1F*) file->Get("h_reco_N_Pi_cos");
  TH1F *h_N_Pi_corr_cos = (TH1F*) file->Get("h_N_Pi_corr_cos");

  TH1F *h_stable_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  TH1F *h_purity_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  h_stable_cos->Divide(h_gen_N_Pi_cos);
  h_purity_cos->Divide(h_reco_N_Pi_cos);

  if( h->GetNbinsX() != h_stable_cos->GetNbinsX() ) throw std::logic_error("Error");
  if( h->GetNbinsX() != h_purity_cos->GetNbinsX() ) throw std::logic_error("Error");

  TH1F *h_weight = (TH1F*) h_stable_cos->Clone();
  h_weight->Divide(h_purity_cos);
  TH1F *corrected = (TH1F*) h->Clone();
  corrected->Divide(h_weight);

  return corrected;

}

void Func2Hist( TH1F * h, double * par )
{
  for ( int ibin=1; ibin < nbins_cos+1; ibin++ ){

    Float_t x = h->GetXaxis()->GetBinCenter(ibin);
    Float_t binc_h = par[0]*(1+x*x)+par[1]*x;
    h->SetBinContent(ibin,binc_h);

  }
}

void main_pq_BGFit( TFile *files[] )
{
  enum MixProcess {kUU,kDD,kUD};
  gStyle->SetOptStat(0);

  // gen uu/dd polar
  TH1F *h_gen_uu_qcos = (TH1F*) files[kUU]->Get("h_gen_q_qcos");
  TH1F *h_gen_dd_qcos = (TH1F*) files[kDD]->Get("h_gen_q_qcos");
  TH1F *h_gen_ud_qcos = (TH1F*) files[kUD]->Get("h_gen_q_qcos");

  StyleHist(h_gen_uu_qcos,kBlue+1);
  h_gen_uu_qcos->SetFillStyle(0);
  h_gen_uu_qcos->SetLineStyle(2);
  StyleHist(h_gen_dd_qcos,kGreen+2);
  h_gen_dd_qcos->SetFillStyle(0);
  h_gen_dd_qcos->SetLineStyle(2);
  StyleHist(h_gen_ud_qcos,kBlack);
  h_gen_ud_qcos->SetFillStyle(0);
  h_gen_ud_qcos->SetLineStyle(2);

  // reco us polar
  TH1F *h_reco_ud_Pi_scos  = (TH1F*) files[kUD]->Get("h_reco_Pi_scos");
  TH1F *h_reco_ud_Pi_qcos  = (TH1F*) files[kUD]->Get("h_reco_Pi_qcos");
  TH1F *h_cheat_ud_Pi_qcos = (TH1F*) files[kUD]->Get("h_cheat_Pi_qcos");

  // efficiency correction
  TH1F *h_reco_ud_Pi_scos_eff_corr  = Efficiency_Correction2(h_reco_ud_Pi_scos,"scos_corr",files[kUD]);
  TH1F *h_reco_ud_Pi_qcos_eff_corr  = Efficiency_Correction2(h_reco_ud_Pi_qcos,"qcos_corr",files[kUD]);
  TH1F *h_cheat_ud_Pi_qcos_eff_corr = Efficiency_Correction2(h_cheat_ud_Pi_qcos,"cheat_qcos_corr",files[kUD]);
  // TH1F *h_reco_ud_Pi_scos_eff_corr = (TH1F*) h_reco_ud_Pi_scos->Clone();
  // TH1F *h_reco_ud_Pi_qcos_eff_corr = (TH1F*) h_reco_ud_Pi_qcos->Clone();

  // used for pq correction
  TH1F *h_acc_PiPi_cos  = (TH1F*) files[kUD]->Get("pq/h_acc_PiPi_cos");
  TH1F *h_rej_PiPi_cos  = (TH1F*) files[kUD]->Get("pq/h_rej_PiPi_cos");

  TH1F *h_acc_PiPi_cos_eff_corr = Efficiency_Correction2(h_acc_PiPi_cos,"acc_corr",files[kUD]);
  TH1F *h_rej_PiPi_cos_eff_corr = Efficiency_Correction2(h_rej_PiPi_cos,"rej_corr",files[kUD]);
  // TH1F *h_acc_PiPi_cos_eff_corr = (TH1F*) h_acc_PiPi_cos->Clone();
  // TH1F *h_rej_PiPi_cos_eff_corr = (TH1F*) h_rej_PiPi_cos->Clone();

  StyleHist(h_reco_ud_Pi_scos_eff_corr,kBlue);
  h_reco_ud_Pi_scos_eff_corr->SetFillStyle(0);
  StyleHist(h_reco_ud_Pi_qcos_eff_corr,kRed+2);
  StyleHist(h_cheat_ud_Pi_qcos_eff_corr,kBlue);
  StyleHist(h_cheat_ud_Pi_qcos,kBlue);

  StyleHist(h_acc_PiPi_cos_eff_corr,kRed+2);
  StyleHist(h_rej_PiPi_cos_eff_corr,kBlue+2);

  const Int_t nbins = h_reco_ud_Pi_scos_eff_corr->GetNbinsX();

  TH1F *p_KK = new TH1F("p_KK", "p_KK", 50,0,1);
  p_KK->Sumw2();

  vector<Float_t> p_vec = GetP(h_acc_PiPi_cos_eff_corr, h_rej_PiPi_cos_eff_corr);

  for (unsigned i = 0; i < p_vec.size() / 2; i++)
  {
    p_KK->SetBinContent(nbins / 2 - i, p_vec.at(i));
    p_KK->SetBinError(nbins / 2 - i, p_vec.at(i + nbins / 2));
  }

  // TH1F *h_reco_Pi_pq_cos = CorrectHist(h_reco_ud_Pi_qcos_eff_corr, p_vec);
  TH1F *h_reco_Pi_pq_cos = (TH1F*) h_reco_ud_Pi_qcos_eff_corr->Clone();
  StyleHist(h_reco_Pi_pq_cos,kBlack);

  Normalize2Reco(h_reco_Pi_pq_cos,h_gen_ud_qcos);
  Normalize2Reco(h_reco_Pi_pq_cos,h_cheat_ud_Pi_qcos);


  // Fit
  // Gen
  TF1 * f_gen_ud      = new TF1("f_gen_ud","[0]*(1+x*x)+[1]*x",-1.0,1.0);
  StyleFunc(f_gen_ud,2,kBlack);
  f_gen_ud->SetParNames("Su","Au");
  h_gen_ud_qcos->Fit("f_gen_ud","MNRS");

  // Reco
  TF1 * f_reco_ud      = new TF1("f_reco_ud","[0]*(1+x*x)+[1]*x",-0.8,0.8);
  StyleFunc(f_reco_ud,1,kRed);
  f_reco_ud->SetParNames("Su","Au");
  h_reco_Pi_pq_cos->Fit("f_reco_ud","MNRS");

  // Draw
  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  TPad *pad1 = new TPad("pad1", "pad1",0,0,1,1);
  StylePad(pad1,0,0.12,0,0.12);

  h_reco_Pi_pq_cos->SetTitle(";cos#theta_{#pi};Entries");
  h_reco_Pi_pq_cos->GetYaxis()->SetRangeUser(0,30E3);

  h_reco_Pi_pq_cos->Draw("h");
  // h_reco_ud_Pi_scos_eff_corr->Draw("hsame");
  h_cheat_ud_Pi_qcos->Draw("hsame");
  // h_cheat_ud_Pi_qcos_eff_corr->Draw("hsame");
  h_gen_ud_qcos->Draw("hsame");
  f_gen_ud->Draw("same");
  f_reco_ud->Draw("same");

  cout << "Gen  Chi2 / ndf = " << f_gen_ud->GetChisquare() << " / " << f_gen_ud->GetNDF() << endl;
  cout << "Reco Chi2 / ndf = " << f_reco_ud->GetChisquare() << " / " << f_reco_ud->GetNDF() << endl;

  TLegend *leg = new TLegend(0.4,0.70,0.8,0.85);
  leg->SetLineColor(0);
  leg->AddEntry(h_reco_Pi_pq_cos,"Reco #pi angle","f");
  leg->AddEntry(h_gen_ud_qcos,"Gen #pi angle","f");
  leg->AddEntry(h_cheat_ud_Pi_qcos,"Cheated #pi angle","f");
  leg->AddEntry(f_reco_ud,"Reco #pi angle Fit","l");
  leg->AddEntry(f_gen_ud,"Gen #pi angle Fit","l");
  leg->Draw();

  // Plot p value
  TCanvas *c_pval = new TCanvas("c_pval","c_pval",800,800);
  TPad *pad_pval = new TPad("pad_pval", "pad_pval",0,0,1,1);
  StylePad(pad_pval,0,0.12,0,0.15);
  
  StyleHist(p_KK,kGreen+2);
  p_KK->SetTitle(";cos#theta_{#pi};p value");
  p_KK->GetYaxis()->SetRangeUser(0,1);
  p_KK->Draw("h");

}

void pq_method_PiBGFit_norm()
{
  TGaxis::SetMaxDigits(3);

  try
  {
    TString process[3] = {"uu","dd","ud"};
    TFile* files[3];
    for ( int i=0; i<3; i++ ){
      files[i] = new TFile(TString::Format("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.%s.KPiLPFO.distPi0.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root",process[i].Data()),"READ");
      if ( !files[i]->IsOpen() ) throw 0;
    }

    main_pq_BGFit( files );
  }
  catch ( int error_code ) {
    switch ( error_code ){
      default:
        cerr << "<< Error >>" << endl;
        break;
    }
  }

}